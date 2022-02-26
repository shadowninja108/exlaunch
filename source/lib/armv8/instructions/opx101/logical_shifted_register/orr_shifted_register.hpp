#pragma once

#include "base.hpp"

namespace exl::armv8::inst {

    struct OrrShiftedRegister : public impl::opx101::LogicalShiftedRegister {
        
        static constexpr u8 Sf = 0b1;
        static constexpr u8 Opc = 0b01;

        ACCESSOR(Shift, 22, 24);
        ACCESSOR(Rm,    16, 21);
        ACCESSOR(Imm6,  10, 16);

        enum ShiftType : u8 {
            LSL = 0b00,
            LSR = 0b01,
            ASR = 0b10,
            ROR = 0b11,
        };

        constexpr inline static auto GetSf(reg::Register rd, reg::Register rn, reg::Register rm) {
            /* TODO: static_assert(rd.Is64() == rn.Is64() && rn.Is64() == rm.Is64(), ""); */
            return rd.Is64();
        }

        constexpr OrrShiftedRegister(reg::Register rd, reg::Register rn, reg::Register rm, ShiftType shift, u16 amount)
        : LogicalShiftedRegister(GetSf(rd, rn, rm), Opc) {
            SetShift(shift);
            SetN(0);
            SetRm(rm.Index());
            SetImm6(amount);
            SetRn(rn.Index());
            SetRd(rd.Index());
        }

    };

}

#include "lsl.hpp"
#include "lsr.hpp"
#include "asr.hpp"
#include "ror.hpp"