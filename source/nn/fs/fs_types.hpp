#pragma once

#include "nn/nn_common.hpp"

namespace nn::fs {

    /* Handle representing an opened file. */
    struct FileHandle {
        u64 _internal;
    };
    
    /* Mode for opening files. */
    enum OpenMode {
        OpenMode_Read   = BIT(0),
        OpenMode_Write  = BIT(1),
        OpenMode_Append = BIT(2),

        OpenMode_ReadWrite  = OpenMode_Read | OpenMode_Write;
    };

    /* Options for reading. TODO: is this an enum? what for? */
    struct ReadOption {
        int _field_0;

        static inline ReadOption MakeOption(int value) {
            return {value};
        }
    };
};