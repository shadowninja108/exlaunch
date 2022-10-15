#pragma once

#include <lib/armv8.hpp>

namespace exl::armv8::inst::impl {

    struct Op101xInstruction : public Instruction {

        ACCESSOR(Op0, 29, 32);
        ACCESSOR(Op1, 12, 26);
        ACCESSOR(Op2, 0, 5);

        constexpr Op101xInstruction(u8 op0) : Instruction(0b1010) {
            SetOp0(op0);
        }
    };
}

#include "hints/base.hpp"
#include "unconditional_branch_immediate/base.hpp"
#include "unconditional_branch_register/base.hpp"