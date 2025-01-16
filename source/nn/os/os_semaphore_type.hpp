#pragma once

#include "impl/os_internal_condition_variable.hpp"
#include "impl/os_internal_critical_section.hpp"

namespace nn::os {
    
    struct SemaphoreType {
        SemaphoreType* mPrev;
        SemaphoreType* mNext;
        char mState;
        int mLockCount;
        int field_18;
        detail::InternalCriticalSection mCS;
        detail::InternalConditionVariable mCV;
    };
}