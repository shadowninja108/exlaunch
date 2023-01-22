#pragma once

#include <lib/armv8.hpp>
#include "util/math/sign_extend.hpp"

namespace exl::armv8::inst::impl::opx1x0 {

    struct LoadStoreRegisterUnsignedImmediate : public Opx1x0Instruction {

        static constexpr u8  Op0 = 0b0011;
        static constexpr u8  Op2 = 0b10;

        ACCESSOR(Size,      30, 32);
        ACCESSOR(V,         26);
        ACCESSOR(Opc,       22, 24);
        ACCESSOR(Imm12,     10, 22);
        ACCESSOR(Rn,        5, 10);
        ACCESSOR(Rt,        0, 5);

        constexpr LoadStoreRegisterUnsignedImmediate(u8 size, u8 v, u8 opc, u16 imm12, reg::Register rn, reg::Register rt) : Opx1x0Instruction(Op0) {
            SetOp2(Op2);
            SetSize(size);
            SetV(v);
            SetOpc(opc);
            SetImm12(imm12);
            SetRn(rn.Index());
            SetRt(rt.Index());
        }
    };
}

#include "ldr_register_immediate.hpp"
#include "str_register_immediate.hpp"