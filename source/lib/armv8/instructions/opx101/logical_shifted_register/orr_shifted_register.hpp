#pragma once

#include "base.hpp"

namespace exl::armv8::inst {

    struct OrrShiftedRegister : public impl::opx101::LogicalShiftedRegister {
        
        static constexpr u8 Sf = 0b1;
        static constexpr u8 Opc = 0b01;

        ACCESSOR(Shift, 22, 24);
        ACCESSOR(Rm,    16, 21);
        ACCESSOR(Imm6,  10, 16);

        constexpr inline static auto GetSf(reg::Register rd, reg::Register rn, reg::Register rm) {
            /* TODO: static_assert(rd.Is64() == rn.Is64() && rn.Is64() == rm.Is64(), ""); */
            return rd.Is64();
        }

        constexpr OrrShiftedRegister(reg::Register rd, reg::Register rn, reg::Register rm, ShiftType shift = ShiftType_LSL, u16 amount = 0)
        : LogicalShiftedRegister(GetSf(rd, rn, rm), Opc) {
            SetShift(shift);
            SetN(0);
            SetRm(rm.Index());
            SetImm6(amount);
            SetRn(rn.Index());
            SetRd(rd.Index());
        }

    };
    
    static_assert(OrrShiftedRegister(reg::X0, reg::X1,  reg::X2).Value()    == 0xAA020020, "");
    static_assert(OrrShiftedRegister(reg::X3, reg::X4,  reg::X5).Value()    == 0xAA050083, "");
    static_assert(OrrShiftedRegister(reg::X6, reg::X7,  reg::X8).Value()    == 0xAA0800E6, "");
    static_assert(OrrShiftedRegister(reg::X9, reg::X10, reg::X11).Value()   == 0xAA0B0149, "");

    static_assert(OrrShiftedRegister(reg::X0, reg::X1,  reg::X2,    ShiftType_LSR, 8).Value()   == 0xAA422020, "");
    static_assert(OrrShiftedRegister(reg::X3, reg::X4,  reg::X5,    ShiftType_LSR, 8).Value()   == 0xAA452083, "");
    static_assert(OrrShiftedRegister(reg::X6, reg::X7,  reg::X8,    ShiftType_LSR, 8).Value()   == 0xAA4820E6, "");
    static_assert(OrrShiftedRegister(reg::X9, reg::X10, reg::X11,   ShiftType_LSR, 8).Value()   == 0xAA4B2149, "");

    static_assert(OrrShiftedRegister(reg::X0, reg::X1,  reg::X2,    ShiftType_ASR, 8).Value()   == 0xAA822020, "");
    static_assert(OrrShiftedRegister(reg::X3, reg::X4,  reg::X5,    ShiftType_ASR, 8).Value()   == 0xAA852083, "");
    static_assert(OrrShiftedRegister(reg::X6, reg::X7,  reg::X8,    ShiftType_ASR, 8).Value()   == 0xAA8820E6, "");
    static_assert(OrrShiftedRegister(reg::X9, reg::X10, reg::X11,   ShiftType_ASR, 8).Value()   == 0xAA8B2149, "");

    static_assert(OrrShiftedRegister(reg::X0, reg::X1,  reg::X2,    ShiftType_ROR, 8).Value()   == 0xAAC22020, "");
    static_assert(OrrShiftedRegister(reg::X3, reg::X4,  reg::X5,    ShiftType_ROR, 8).Value()   == 0xAAC52083, "");
    static_assert(OrrShiftedRegister(reg::X6, reg::X7,  reg::X8,    ShiftType_ROR, 8).Value()   == 0xAAC820E6, "");
    static_assert(OrrShiftedRegister(reg::X9, reg::X10, reg::X11,   ShiftType_ROR, 8).Value()   == 0xAACB2149, "");

}


#include "mov_register.hpp"