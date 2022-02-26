#pragma once

#include "base.hpp"

namespace exl::armv8::inst {

    struct Movz : public impl::op100x::MoveWideImmediate {

        static constexpr u8 Opc = 0b10;
        static constexpr u8 Hw = 0b00;

        constexpr Movz(reg::Register reg, u16 imm) : MoveWideImmediate(reg, Opc, Hw, imm) {}

    };
}