#pragma once

#include "base.hpp"

namespace exl::armv8::inst {

    struct BranchRegister : public impl::op101x::UnconditionalBranchRegister {

        static constexpr u8 Opc = 0b0000;
        static constexpr u8 Op2 = 0b11111;
        static constexpr u8 Op3 = 0b000000;
        static constexpr u8 Op4 = 0b00000;

        constexpr BranchRegister(reg::Register rn) : UnconditionalBranchRegister(Opc, Op2) {
            /*
                static_assert(rn.Is64());
            */

            SetOp3(Op3);
            SetRn(rn.Index());
            SetOp4(Op4);
        }
    };

    static_assert(BranchRegister(reg::X0).Value() == 0xD61F0000, "");
    static_assert(BranchRegister(reg::X1).Value() == 0xD61F0020, "");
    static_assert(BranchRegister(reg::X2).Value() == 0xD61F0040, "");
    static_assert(BranchRegister(reg::X3).Value() == 0xD61F0060, "");
}