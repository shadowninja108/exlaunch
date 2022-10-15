#pragma once

#include "base.hpp"

namespace exl::armv8::inst {

    struct Adrp : public impl::op100x::PcRelAddressing {
        
        static constexpr u8 Opc = 0b00;
        static constexpr u8 Hw = 0b00;

        constexpr Adrp(reg::Register reg, u32 imm) : PcRelAddressing(reg, imm >> 12, Op_ADRP) {}

    };

    static_assert(Adrp(reg::X0, 0x1000).Value()     == 0xB0000000, "");
    static_assert(Adrp(reg::X1, 0xfff000).Value()   == 0xF0007FE1, "");
    static_assert(Adrp(reg::X2, 0x6969000).Value()  == 0xB0034B42, "");
}