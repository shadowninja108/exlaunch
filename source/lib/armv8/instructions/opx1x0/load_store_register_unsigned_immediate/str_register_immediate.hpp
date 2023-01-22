#pragma once

#include "base.hpp"

namespace exl::armv8::inst {

    struct StrRegisterImmediate : public impl::opx1x0::LoadStoreRegisterUnsignedImmediate {

        static constexpr bool V = 0b0;
        static constexpr u8 Opc = 0b00;

        static constexpr u8 GetSize(reg::Register rt) {
            return 0b10 | rt.Is64();
        }

        constexpr StrRegisterImmediate(reg::Register rt, reg::Register rn, u16 imm12 = 0) : LoadStoreRegisterUnsignedImmediate(
            GetSize(rt), V, Opc, imm12, rn, rt
        ) {}
    };

    static_assert(StrRegisterImmediate(reg::X0, reg::X1).Value()        == 0xF9000020, "");
    static_assert(StrRegisterImmediate(reg::X2, reg::X3).Value()        == 0xF9000062, "");
    static_assert(StrRegisterImmediate(reg::X4, reg::X5).Value()        == 0xF90000A4, "");
    static_assert(StrRegisterImmediate(reg::X6, reg::X7).Value()        == 0xF90000E6, "");
    static_assert(StrRegisterImmediate(reg::X8, reg::X9).Value()        == 0xF9000128, "");
    static_assert(StrRegisterImmediate(reg::X10, reg::X11, 1).Value()  == 0xF900056A, "");
    static_assert(StrRegisterImmediate(reg::X12, reg::X13, 2).Value()   == 0xF90009AC, "");
    static_assert(StrRegisterImmediate(reg::X14, reg::X15, 4).Value()   == 0xF90011EE, "");
    static_assert(StrRegisterImmediate(reg::W16, reg::X17, 20).Value()  == 0xB9005230, "");
    static_assert(StrRegisterImmediate(reg::W18, reg::X19, 52).Value()  == 0xB900D272, "");
    static_assert(StrRegisterImmediate(reg::W20, reg::X21, 41).Value()  == 0xB900A6B4, "");
    static_assert(StrRegisterImmediate(reg::W22, reg::X23, 75).Value()  == 0xB9012EF6, "");
    static_assert(StrRegisterImmediate(reg::W24, reg::X25, 95).Value()  == 0xB9017F38, "");
    static_assert(StrRegisterImmediate(reg::W26, reg::X27, 69).Value()  == 0xB901177A, "");
    static_assert(StrRegisterImmediate(reg::W28, reg::X29).Value()      == 0xB90003BC, "");
    static_assert(StrRegisterImmediate(reg::X30, reg::SP).Value()       == 0xF90003FE, "");
    static_assert(StrRegisterImmediate(reg::LR, reg::SP).Value()        == 0xF90003FE, "");
}