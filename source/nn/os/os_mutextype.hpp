#pragma once

#include "nn/nn_common.hpp"

namespace nn::os {

    struct ThreadType;
    struct MutexType {

        enum State {
            State_NotInitialized = 0,
            State_Initialized    = 1,
        };

        u8 m_State;
        bool m_IsRecursive;
    };
};