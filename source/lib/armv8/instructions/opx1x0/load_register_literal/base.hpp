#pragma once

#include <lib/armv8.hpp>
#include "util/math/sign_extend.hpp"

namespace exl::armv8::inst::impl::opx1x0 {

    struct LoadRegisterLiteral : public Opx1x0Instruction {

        static constexpr u8  Op0 = 0b0001;
        static constexpr u8  Op2 = 0b00;

        ACCESSOR(Opc,       30, 31);
        ACCESSOR(V,         26);
        ACCESSOR(Imm19,     5, 24);
        ACCESSOR(Rt,        0, 5);

        constexpr LoadRegisterLiteral(reg::Register rt, int imm19, u8 v, u8 opc) : Opx1x0Instruction(Op0) {
            SetOp0(Op0);
            SetOp2(Op2);
            SetOpc(opc);
            SetV(v);
            SetImm19(util::SignExtend<Imm19Mask.Count>(imm19));
            SetRt(rt.Index());
        }
    };
}

#include "ldr_literal.hpp"