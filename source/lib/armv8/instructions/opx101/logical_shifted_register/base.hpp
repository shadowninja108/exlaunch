#pragma once

#include <lib/armv8.hpp>

namespace exl::armv8::inst::impl::opx101 {

    struct LogicalShiftedRegister : public Opx101Instruction {

        static constexpr u8 Op0 = 0b0;
        static constexpr u8 Op1 = 0b0;
        static constexpr u8 Op2 = 0b0000;
        static constexpr u8 Op3 = 0b000000;

        ACCESSOR(Sf,    31);
        ACCESSOR(Opc,   29, 31);
        ACCESSOR(N,     21);
        ACCESSOR(Immr,  16, 22);
        ACCESSOR(Imms,  10, 16);
        ACCESSOR(Rn,    5, 10);
        ACCESSOR(Rd,    0, 5);

        constexpr LogicalShiftedRegister(u8 sf, u8 opc) : Opx101Instruction(Op0, Op1, Op2, Op3) {
            SetSf(sf);
            SetOpc(opc);
        }
    };
};

#include "orr_shifted_register.hpp"