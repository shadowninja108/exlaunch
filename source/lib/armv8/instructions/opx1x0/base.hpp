#pragma once

#include <lib/armv8.hpp>

namespace exl::armv8::inst::impl {

    struct Opx1x0Instruction : public Instruction {

        ACCESSOR(Op0, 28, 32);
        ACCESSOR(Op1, 26);
        ACCESSOR(Op2, 23, 25);
        ACCESSOR(Op3, 16, 22);
        ACCESSOR(Op4, 10, 12);

        constexpr Opx1x0Instruction(u8 op0) : Instruction(0b0100) {
            SetOp0(op0);
        }
    };
}

#include "load_store_register_offset/base.hpp"
#include "load_store_register_unscaled_immediate/base.hpp"
#include "load_store_register_unsigned_immediate/base.hpp"