#pragma once

#include "base.hpp"

namespace exl::armv8::inst {

    struct LdurUnscaledImmediate : public impl::opx1x0::LoadStoreRegisterUnscaledImmediate {

        static constexpr bool V = 0b0;
        static constexpr u8 Opc = 0b01;

        static constexpr u8 GetSize(reg::Register rt) {
            return 0b10 | rt.Is64();
        }

        constexpr LdurUnscaledImmediate(reg::Register rt, reg::Register rn, s16 imm9 = 0) : LoadStoreRegisterUnscaledImmediate(
            GetSize(rt), V, Opc, imm9, rn, rt
        ) {}
    };

    static_assert(LdurUnscaledImmediate(reg::X0, reg::X1, -1).Value()       == 0xF85FF020, "");
    static_assert(LdurUnscaledImmediate(reg::X2, reg::X3, 1).Value()        == 0xF8401062, "");
    static_assert(LdurUnscaledImmediate(reg::X4, reg::X5, -2).Value()       == 0xF85FE0A4, "");
    static_assert(LdurUnscaledImmediate(reg::X6, reg::X7, 2).Value()        == 0xF84020E6, "");
    static_assert(LdurUnscaledImmediate(reg::X8, reg::X9, -3).Value()       == 0xF85FD128, "");
    static_assert(LdurUnscaledImmediate(reg::X10, reg::X11, -4).Value()     == 0xF85FC16A, "");
    static_assert(LdurUnscaledImmediate(reg::X12, reg::X13, 2).Value()      == 0xF84021AC, "");
    static_assert(LdurUnscaledImmediate(reg::X14, reg::X15, -4).Value()     == 0xF85FC1EE, "");
    static_assert(LdurUnscaledImmediate(reg::W16, reg::X17, 20).Value()     == 0xB8414230, "");
    static_assert(LdurUnscaledImmediate(reg::W18, reg::X19, -52).Value()    == 0xB85CC272, "");
    static_assert(LdurUnscaledImmediate(reg::W20, reg::X21, 41).Value()     == 0xB84292B4, "");
    static_assert(LdurUnscaledImmediate(reg::W22, reg::X23, -75).Value()    == 0xB85B52F6, "");
    static_assert(LdurUnscaledImmediate(reg::W24, reg::X25, 95).Value()     == 0xB845F338, "");
    static_assert(LdurUnscaledImmediate(reg::W26, reg::X27, -69).Value()    == 0xB85BB37A, "");
    static_assert(LdurUnscaledImmediate(reg::W28, reg::X29, -1).Value()     == 0xB85FF3BC, "");
    static_assert(LdurUnscaledImmediate(reg::X30, reg::SP, -5).Value()      == 0xF85FB3FE, "");
    static_assert(LdurUnscaledImmediate(reg::LR, reg::SP, -5).Value()       == 0xF85FB3FE, "");
}