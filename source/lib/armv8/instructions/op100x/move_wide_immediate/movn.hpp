#pragma once

#include "base.hpp"

namespace exl::armv8::inst {

    struct Movn : public impl::op100x::MoveWideImmediate {
        
        static constexpr u8 Opc = 0b00;

        constexpr Movn(reg::Register reg, u16 imm, ShiftValue shift = ShiftValue_0) : MoveWideImmediate(reg, Opc, shift, imm) {}

    };

    static_assert(Movn(reg::X0, 0x10).Value()                   == 0x92800200, "");
    static_assert(Movn(reg::W1, 0x15).Value()                   == 0x128002A1, "");
    static_assert(Movn(reg::X2, 0x12).Value()                   == 0x92800242, "");
    static_assert(Movn(reg::X3, 0x555).Value()                  == 0x9280AAA3, "");
    static_assert(Movn(reg::W3, 0x555, ShiftValue_16).Value()   == 0x12A0AAA3, "");
    static_assert(Movn(reg::W3, 0x235, ShiftValue_16).Value()   == 0x12A046A3, "");
    static_assert(Movn(reg::X7, 0x653, ShiftValue_32).Value()   == 0x92C0CA67, "");
    static_assert(Movn(reg::X8, 0x652, ShiftValue_32).Value()   == 0x92C0CA48, "");
    static_assert(Movn(reg::X9, 0x542, ShiftValue_48).Value()   == 0x92E0A849, "");
    static_assert(Movn(reg::X6, 0x634, ShiftValue_48).Value()   == 0x92E0C686, "");
}