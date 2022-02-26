#pragma once

#include <lib/armv8.hpp>

namespace exl::armv8::inst::impl::op100x {

    struct MoveWideImmediate : public Op100xInstruction {

        static constexpr u8 Op0 = 0b101;

        ACCESSOR(Sf,    31);
        ACCESSOR(Opc,   29, 31);
        ACCESSOR(Hw,    21, 23);
        ACCESSOR(Imm16, 5, 21);
        ACCESSOR(Rd,    0, 5);

        constexpr MoveWideImmediate(reg::Register reg, u8 opc, u8 hw, u16 imm) : Op100xInstruction(Op0) {
            SetSf(reg.Is64());
            SetOpc(opc);
            SetHw(hw);
            SetImm16(imm);
            SetRd(reg.Index());
        }
    };
};

#include "movk.hpp"
#include "movn.hpp"
#include "movz.hpp"