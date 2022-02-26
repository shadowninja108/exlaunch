#pragma once

#include <lib/armv8.hpp>

namespace exl::armv8::inst::impl {

    struct Opx101Instruction : public Instruction {

        ACCESSOR(Op0, 30);
        ACCESSOR(Op1, 28);
        ACCESSOR(Op2, 20, 24);
        ACCESSOR(Op3, 10, 15);

        constexpr Opx101Instruction(u8 op0, u8 op1, u8 op2, u8 op3) : Instruction(0b0101) {
            SetOp0(op0);
            SetOp1(op1);
            SetOp2(op2);
            SetOp3(op3);
        }
    };
}

#include "logical_shifted_register/base.hpp"