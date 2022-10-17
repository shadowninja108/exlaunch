#pragma once

#include "base.hpp"

namespace exl::armv8::inst {

    /* Alias. */
    struct CmnImmediate : public AddsImmediate {

        static constexpr reg::Register GetRd(reg::Register reg) {
            if(reg.Is64()) {
                return reg::None64;
            }
            else {
                return reg::None32;
            }
        }

        constexpr CmnImmediate(reg::Register reg, u32 imm) : AddsImmediate(GetRd(reg), reg, imm) {}

    };

    static_assert(CmnImmediate(reg::X0, 45).Value()         == 0xB100B41F, "");
    static_assert(CmnImmediate(reg::W1, 32).Value()         == 0x3100803F, "");
    static_assert(CmnImmediate(reg::X2, 0x4000).Value()     == 0xB140105F, "");
    static_assert(CmnImmediate(reg::X3, 0x54000).Value()    == 0xB141507F, "");
}