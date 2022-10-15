#pragma once

#include "patcher_impl.hpp"
#include "armv8.hpp"

namespace exl::patch {
    
    class StreamPatcher : public PatcherImpl {
        private:
        constexpr bool DoneNothing() const {
            return m_Start == m_Current;
        }

        inline void Flush() {
            /* Nothing to flush if there was nothing done. */
            if(DoneNothing())
                return;

            /* Get actual pointers. */
            void* ro = (void*) RoFromAddr(m_Start);
            void* rw = (void*) RwFromAddr(m_Start);
            auto size = m_Current - m_Start;

            /* Flush data/instructions. */
            armDCacheFlush(rw, size);
            armICacheInvalidate(ro, size);

            /* Reset start position. */
            m_Start += size;
        }

        protected:
        inline ptrdiff_t RelativeAddressFromBase(uintptr_t address) const {
            return address - m_Current;
        }
        inline ptrdiff_t RelativeAddressFromPointer(void* ptr) const {
            return AddrFromRoPointer(ptr) - m_Current;
        }
        
        uintptr_t m_Start;
        uintptr_t m_Current;

        public:
        inline StreamPatcher(uintptr_t start) : m_Start(start), m_Current(start) {}

        template<typename T>
        inline void Write(T v) {
            At<T>(m_Current) = v;

            m_Current += sizeof(T);
        }

        /* Flush current data then move to a new address. */
        inline void SeekRel(uintptr_t address) {
            /* Don't need to do anything if the address doesn't need to change. */
            if(address == 0)
                return;

            /* Get address relative to base. */
            address += m_Current;

            /* Flush what we've already done. */
            Flush();

            /* Reset to the position to the provided address. */
            m_Start = address;
            m_Current = address;
        }

        /* Address relative to the base (Ro). */
        inline void Seek(uintptr_t address) {
            SeekRel(RelativeAddressFromBase(address));
        }
        /* Absolute address. */
        inline void Seek(void* ptr) {
            SeekRel(RelativeAddressFromPointer(ptr));
        }

        inline ~StreamPatcher() {
            Flush();
        }
    };
}