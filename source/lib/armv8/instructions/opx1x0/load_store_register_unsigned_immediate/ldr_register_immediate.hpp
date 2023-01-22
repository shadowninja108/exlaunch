#pragma once

#include "base.hpp"

namespace exl::armv8::inst {

    struct LdrRegisterImmediate : public impl::opx1x0::LoadStoreRegisterUnsignedImmediate {

        static constexpr bool V = 0b0;
        static constexpr u8 Opc = 0b01;

        static constexpr u8 GetSize(reg::Register rt) {
            return 0b10 | rt.Is64();
        }

        constexpr LdrRegisterImmediate(reg::Register rt, reg::Register rn, u16 imm12 = 0) : LoadStoreRegisterUnsignedImmediate(
            GetSize(rt), V, Opc, imm12, rn, rt
        ) {}
    };

    static_assert(LdrRegisterImmediate(reg::X0, reg::X1).Value()        == 0xF9400020, "");
    static_assert(LdrRegisterImmediate(reg::X2, reg::X3).Value()        == 0xF9400062, "");
    static_assert(LdrRegisterImmediate(reg::X4, reg::X5).Value()        == 0xF94000A4, "");
    static_assert(LdrRegisterImmediate(reg::X6, reg::X7).Value()        == 0xF94000E6, "");
    static_assert(LdrRegisterImmediate(reg::X8, reg::X9).Value()        == 0xF9400128, "");
    static_assert(LdrRegisterImmediate(reg::X10, reg::X11, 1).Value()   == 0xF940056A, "");
    static_assert(LdrRegisterImmediate(reg::X12, reg::X13, 2).Value()   == 0xF94009AC, "");
    static_assert(LdrRegisterImmediate(reg::X14, reg::X15, 4).Value()   == 0xF94011EE, "");
    static_assert(LdrRegisterImmediate(reg::W16, reg::X17, 20).Value()  == 0xB9405230, "");
    static_assert(LdrRegisterImmediate(reg::W18, reg::X19, 52).Value()  == 0xB940D272, "");
    static_assert(LdrRegisterImmediate(reg::W20, reg::X21, 41).Value()  == 0xB940A6B4, "");
    static_assert(LdrRegisterImmediate(reg::W22, reg::X23, 75).Value()  == 0xB9412EF6, "");
    static_assert(LdrRegisterImmediate(reg::W24, reg::X25, 95).Value()  == 0xB9417F38, "");
    static_assert(LdrRegisterImmediate(reg::W26, reg::X27, 69).Value()  == 0xB941177A, "");
    static_assert(LdrRegisterImmediate(reg::W28, reg::X29).Value()      == 0xB94003BC, "");
    static_assert(LdrRegisterImmediate(reg::X30, reg::SP).Value()       == 0xF94003FE, "");
    static_assert(LdrRegisterImmediate(reg::LR, reg::SP).Value()        == 0xF94003FE, "");
}