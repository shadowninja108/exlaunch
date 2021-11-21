#pragma once

#include "common.hpp"

namespace exl {
    class MemLayout {
        private:
        uintptr_t FindTargetAsRtld();
        uintptr_t FindTargetFromRtld();

        public:
        uintptr_t m_SelfStart;
        uintptr_t m_TargetStart;

        void Initialize();
    };
};