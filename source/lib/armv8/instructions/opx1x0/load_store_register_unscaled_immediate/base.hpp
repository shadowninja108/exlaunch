#pragma once

#include <lib/armv8.hpp>
#include "util/math/sign_extend.hpp"

namespace exl::armv8::inst::impl::opx1x0 {

    struct LoadStoreRegisterUnscaledImmediate : public Opx1x0Instruction {

        static constexpr u8  Op0 = 0b0011;
        static constexpr u8  Op2 = 0b00;
        static constexpr u8  Op3 = 0b000000;
        static constexpr u8  Op4 = 0b00;

        ACCESSOR(Size,      30, 32);
        ACCESSOR(V,         26);
        ACCESSOR(Opc,       22, 24);
        ACCESSOR(Imm9,      12, 21);
        ACCESSOR(Rn,        5, 10);
        ACCESSOR(Rt,        0, 5);

        constexpr LoadStoreRegisterUnscaledImmediate(u8 size, u8 v, u8 opc, s16 imm9, reg::Register rn, reg::Register rt) : Opx1x0Instruction(Op0) {
            SetOp2(Op2);
            SetOp3(Op3);
            SetOp4(Op4);
            SetSize(size);
            SetV(v);
            SetOpc(opc);
            SetImm9(util::SignExtend<Imm9Mask.Count>(imm9));
            SetRn(rn.Index());
            SetRt(rt.Index());
        }
    };
}

#include "ldur_unscaled_immediate.hpp"
#include "stur_unscaled_immediate.hpp"