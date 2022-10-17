#pragma once

#include <lib/armv8.hpp>

namespace exl::armv8::inst::impl {

    struct Op100xInstruction : public Instruction {

        ACCESSOR(Op0, 23, 26);

        constexpr Op100xInstruction(u8 op0) : Instruction(0b1000) {
            SetOp0(op0);
        }
    };
}

#include "add_subtract_immediate/base.hpp"
#include "logical_immediate/base.hpp"
#include "move_wide_immediate/base.hpp"
#include "pc_rel_addressing/base.hpp"