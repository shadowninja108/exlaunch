#pragma once

#include "base.hpp"

namespace exl::armv8::inst {

    struct Movk : public impl::op100x::MoveWideImmediate {
        
        static constexpr u8 Opc = 0b11;

        constexpr Movk(reg::Register reg, u16 imm, ShiftValue shift = ShiftValue_0) : MoveWideImmediate(reg, Opc, shift, imm) {}

    };

    static_assert(Movk(reg::X0, 0x10).Value()                   == 0xF2800200, "");
    static_assert(Movk(reg::W1, 0x15).Value()                   == 0x728002A1, "");
    static_assert(Movk(reg::X2, 0x12).Value()                   == 0xF2800242, "");
    static_assert(Movk(reg::X3, 0x555).Value()                  == 0xF280AAA3, "");
    static_assert(Movk(reg::W3, 0x555, ShiftValue_16).Value()   == 0x72A0AAA3, "");
    static_assert(Movk(reg::W3, 0x235, ShiftValue_16).Value()   == 0x72A046A3, "");
    static_assert(Movk(reg::X7, 0x653, ShiftValue_32).Value()   == 0xF2C0CA67, "");
    static_assert(Movk(reg::X8, 0x652, ShiftValue_32).Value()   == 0xF2C0CA48, "");
    static_assert(Movk(reg::X9, 0x542, ShiftValue_48).Value()   == 0xF2E0A849, "");
    static_assert(Movk(reg::X6, 0x634, ShiftValue_48).Value()   == 0xF2E0C686, "");
}