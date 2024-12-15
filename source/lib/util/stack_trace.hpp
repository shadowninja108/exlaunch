#pragma once

#include <common.hpp>

namespace exl::util::stack_trace {
    struct Frame {
        NON_COPYABLE(Frame);
        NON_MOVEABLE(Frame);

        uintptr_t m_Fp;
        uintptr_t m_Lr;
    };

    ALWAYS_INLINE uintptr_t GetSp() {
        return reinterpret_cast<uintptr_t>(__builtin_stack_address());
    }
    ALWAYS_INLINE uintptr_t GetFp() {
        return reinterpret_cast<uintptr_t>(__builtin_frame_address(0));
    }

    ALWAYS_INLINE Frame* GetCurrentFrame() {
        return reinterpret_cast<Frame*>(GetFp());
    }

    template<typename Derived>
    class AbstractIterator {

        protected:
        uintptr_t m_Fp;
        uintptr_t m_Lr;
        uintptr_t m_PrevFp;
        Range m_Stack;

        Derived* GetThis() { return static_cast<Derived*>(this); }
        const Derived* GetThis() const { return static_cast<const Derived*>(this); }

        inline bool IsValidLrImpl(uintptr_t lr) const {
            /* Has to not be null. */
            if(lr == 0)
                return false;

            /* Has to be aligned to an instruction. */
            if((lr % sizeof(exl::armv8::InstType)) != 0)
                return false;

            return GetThis()->IsValidLr(lr);
        }

        public:
        AbstractIterator(uintptr_t fp, uintptr_t lr, Range stack) : 
            m_Fp(fp), 
            m_Lr(lr),
            m_PrevFp(0), 
            m_Stack(stack) 
        {}

        inline uintptr_t GetStackPointer() const {
            if(m_Fp == 0)
                return m_Stack.GetEnd() - sizeof(m_Fp);

            return m_Fp - sizeof(m_Fp);
        }

        inline uintptr_t GetReturnAddress() const {
            return m_Lr;
        }

        inline bool IsValid() const {
            /* FP needs to not be null. */
            if(m_Fp == 0)
                return false;

            /* FP needs to be aligned. */
            if((m_Fp % sizeof(uintptr_t)) != 0)
                return false;

            /* FP needs to be on the stack. */
            if (!m_Stack.InRange(m_Fp))
                return false;

            /* FP needs to be going backwards. */
            if(m_Fp <= m_PrevFp)
                return false;

            return true;
        }

        inline bool Step() {
            if(!IsValid())
                return false;

            auto frame = reinterpret_cast<const Frame*>(m_Fp);

            /* Check if the frame is pointing to valid memory. */
            if(!GetThis()->IsValidPointer(frame))
                return false;

            /* Update state. */
            m_PrevFp = m_Fp;
            m_Lr = frame->m_Lr;
            m_Fp = frame->m_Fp;

            /* See if LR is still valid. */
            if(!IsValidLrImpl(m_Lr)) {
                m_Lr = 0;
                return false;
            }

            return true;
        }

        inline bool StepN(size_t depth) {
            for(size_t i = 0; i < depth; i++) {
                if(!Step())
                    return false;
            }

            return true;
        }

        template<typename Delegate>
        ALWAYS_INLINE void Walk(Delegate delegate) {
            while(IsValid()) {
                delegate();
                Step();
            }
        }
    };

    struct Iterator : public AbstractIterator<Iterator> {
        Iterator(uintptr_t fp, Range stack): AbstractIterator(fp, 0, stack) {}

        inline bool IsValidLr(uintptr_t lr) const { return true; }
        inline bool IsValidPointer(void*) const { return true; }
    };
}