#pragma once

#include <lib.hpp>

namespace exl::patch {
    
    class RandomAccessPatcher : public PatcherImpl {
        private:
            uintptr_t m_Lowest = UINTPTR_MAX;
            uintptr_t m_Highest = UINTPTR_MAX;

        inline bool IsUnset() {
            return m_Lowest == UINTPTR_MAX || m_Highest == UINTPTR_MAX;
        }

        inline void SetLowest(uintptr_t addr) {
            if(IsUnset())
                m_Lowest = addr;
            else
                m_Lowest = std::min(m_Lowest, addr);
        }

        inline void SetHighest(uintptr_t addr) {
            if(IsUnset())
                m_Highest = addr;
            else
                m_Highest = std::max(m_Highest, addr);
        }

        public:
        template<typename T>
        void Write(const uintptr_t addr, T value) {
            /* Update low/high. */
            SetLowest(addr);
            SetHighest(addr + sizeof(T));

            /* Write value. */
            At<T>(addr) = value;
        }

        template<typename T>
        T& Read(const uintptr_t index) {
            return At<T>(index);
        }

        inline void Flush() {
            /* Nothing to flush if there was nothing done. */
            if(IsUnset())
                return;

            /* Get actual pointers. */
            void* ro = (void*) RoFromAddr(m_Lowest);
            void* rw = (void*) RwFromAddr(m_Lowest);
            auto size = m_Highest - m_Lowest;

            /* Flush data/instructions. */
            armDCacheFlush(rw, size);
            armICacheInvalidate(ro, size);
        }

        inline ~RandomAccessPatcher() {
            /* Flush out any changes we made. */
            Flush();
        }
    };
};