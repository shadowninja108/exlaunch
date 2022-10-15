#pragma once

#include <lib/armv8.hpp>

namespace exl::armv8::inst {

    struct Instruction : public InstBitSet {

        ACCESSOR(MainOp0, 25, 29);

        constexpr Instruction(u8 op0) {
            SetMainOp0(op0);
        }
    };

    enum ShiftType : u8 {
        ShiftType_LSL = 0b00,
        ShiftType_LSR = 0b01,
        ShiftType_ASR = 0b10,
        ShiftType_ROR = 0b11,
    };

    enum ExtendType : u8 {
        ExtendType_UXTB = 0b000, 
        ExtendType_UXTH = 0b001, 
        ExtendType_UXTW = 0b010, 
        ExtendType_UXTX = 0b011, 
        ExtendType_LSL  = 0b011, 
        ExtendType_SXTB = 0b100, 
        ExtendType_SXTH = 0b101, 
        ExtendType_SXTW = 0b110, 
        ExtendType_SXTX = 0b111, 
    };

}

#include "op100x/base.hpp"
#include "op101x/base.hpp"
#include "opx1x0/base.hpp"
#include "opx101/base.hpp"