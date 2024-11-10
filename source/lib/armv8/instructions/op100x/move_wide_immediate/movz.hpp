#pragma once

#include "base.hpp"

namespace exl::armv8::inst {

    struct Movz : public impl::op100x::MoveWideImmediate {

        static constexpr u8 Opc = 0b10;

        constexpr Movz(reg::Register reg, u16 imm, ShiftValue shift = ShiftValue_0) : MoveWideImmediate(reg, Opc, shift, imm) {}

    };

}