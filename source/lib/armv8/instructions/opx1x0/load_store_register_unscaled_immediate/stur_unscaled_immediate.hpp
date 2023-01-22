#pragma once

#include "base.hpp"

namespace exl::armv8::inst {

    struct SturUnscaledImmediate : public impl::opx1x0::LoadStoreRegisterUnscaledImmediate {

        static constexpr bool V = 0b0;
        static constexpr u8 Opc = 0b00;

        static constexpr u8 GetSize(reg::Register rt) {
            return 0b10 | rt.Is64();
        }

        constexpr SturUnscaledImmediate(reg::Register rt, reg::Register rn, u16 imm12 = 0) : LoadStoreRegisterUnscaledImmediate(
            GetSize(rt), V, Opc, imm12, rn, rt
        ) {}
    };

    static_assert(SturUnscaledImmediate(reg::X0, reg::X1, -1).Value()       == 0xF81FF020, "");
    static_assert(SturUnscaledImmediate(reg::X2, reg::X3, 1).Value()        == 0xF8001062, "");
    static_assert(SturUnscaledImmediate(reg::X4, reg::X5, -2).Value()       == 0xF81FE0A4, "");
    static_assert(SturUnscaledImmediate(reg::X6, reg::X7, 2).Value()        == 0xF80020E6, "");
    static_assert(SturUnscaledImmediate(reg::X8, reg::X9, -3).Value()       == 0xF81FD128, "");
    static_assert(SturUnscaledImmediate(reg::X10, reg::X11, -4).Value()     == 0xF81FC16A, "");
    static_assert(SturUnscaledImmediate(reg::X12, reg::X13, 2).Value()      == 0xF80021AC, "");
    static_assert(SturUnscaledImmediate(reg::X14, reg::X15, -4).Value()     == 0xF81FC1EE, "");
    static_assert(SturUnscaledImmediate(reg::W16, reg::X17, 20).Value()     == 0xB8014230, "");
    static_assert(SturUnscaledImmediate(reg::W18, reg::X19, -52).Value()    == 0xB81CC272, "");
    static_assert(SturUnscaledImmediate(reg::W20, reg::X21, 41).Value()     == 0xB80292B4, "");
    static_assert(SturUnscaledImmediate(reg::W22, reg::X23, -75).Value()    == 0xB81B52F6, "");
    static_assert(SturUnscaledImmediate(reg::W24, reg::X25, 95).Value()     == 0xB805F338, "");
    static_assert(SturUnscaledImmediate(reg::W26, reg::X27, -69).Value()    == 0xB81BB37A, "");
    static_assert(SturUnscaledImmediate(reg::W28, reg::X29, -1).Value()     == 0xB81FF3BC, "");
    static_assert(SturUnscaledImmediate(reg::X30, reg::SP, -5).Value()      == 0xF81FB3FE, "");
    static_assert(SturUnscaledImmediate(reg::LR, reg::SP, -5).Value()       == 0xF81FB3FE, "");
}