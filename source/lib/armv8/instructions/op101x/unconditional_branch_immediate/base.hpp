#pragma once

#include <lib/armv8.hpp>

namespace exl::armv8::inst::impl::op101x {

    struct UnconditionalBranchImmediate : public Op101xInstruction {

        static constexpr u8 Op0 = 0b000;

        ACCESSOR(Op,    31);
        ACCESSOR(Imm26, 0, 26);

        enum Op {
            B = 0,
            BL = 1,
        };

        constexpr UnconditionalBranchImmediate(Op op, uint relative_address) : Op101xInstruction(Op0) {
            SetOp(op);
            SetImm26(relative_address / 4);
        }
    };
}

#include "b.hpp"
#include "bl.hpp"