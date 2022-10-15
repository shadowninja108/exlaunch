#pragma once

#include <lib/armv8.hpp>

namespace exl::armv8::inst::impl::op101x {

    struct UnconditionalBranchRegister : public Op101xInstruction {

        static constexpr u8 Op0 = 0b110;
        static constexpr u32 Op1 = 0b10000000000000;

        ACCESSOR(Opc,   21, 25);
        ACCESSOR(UBROp2,16, 21);
        ACCESSOR(Op3,   10, 16);
        ACCESSOR(Rn,    5,  10);
        ACCESSOR(Op4,   0,  5);

        constexpr UnconditionalBranchRegister(u8 opc, u8 op2) : Op101xInstruction(Op0) {
            SetOp1(Op1);
            SetOpc(opc);
            SetUBROp2(op2);
        }
    };
}

#include "br.hpp"
#include "ret.hpp"