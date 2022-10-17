#pragma once

#include "base.hpp"

namespace exl::armv8::inst {

    /* Alias. */
    struct CmpImmediate : public SubsImmediate {

        static constexpr reg::Register GetRd(reg::Register reg) {
            if(reg.Is64()) {
                return reg::None64;
            }
            else {
                return reg::None32;
            }
        }

        constexpr CmpImmediate(reg::Register reg, u32 imm) : SubsImmediate(GetRd(reg), reg, imm) {}

    };

    static_assert(CmpImmediate(reg::X0, 45).Value()         == 0xF100B41F, "");
    static_assert(CmpImmediate(reg::W1, 32).Value()         == 0x7100803F, "");
    static_assert(CmpImmediate(reg::X2, 0x4000).Value()     == 0xF140105F, "");
    static_assert(CmpImmediate(reg::X3, 0x54000).Value()    == 0xF141507F, "");
}