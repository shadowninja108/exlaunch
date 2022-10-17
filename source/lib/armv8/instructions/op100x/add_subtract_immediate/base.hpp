#pragma once

#include <lib/armv8.hpp>

namespace exl::armv8::inst::impl::op100x {

    struct AddSubtractImmediate : public Op100xInstruction {

        static constexpr u8 Op0 = 0b010;
        
        static const uint ImmShift  = 12;
        static const u32 MaskForImmShift = (1 << ImmShift) - 1;

        ACCESSOR(Sf,    31);
        ACCESSOR(Op,    30);
        ACCESSOR(S,     29);
        ACCESSOR(Sh,    22);
        ACCESSOR(Imm12, 10, 22);
        ACCESSOR(Rn,    5, 10);
        ACCESSOR(Rd,    0, 5);

        constexpr AddSubtractImmediate(bool sf, bool op, bool s) : Op100xInstruction(Op0) {
            SetSf(sf);
            SetOp(op);
            SetS(s);
        }

        static constexpr bool CalcSh(u32 imm) {
            return imm != 0 && (imm & MaskForImmShift) == 0;
        }

        static constexpr u16 CalcImm(u32 imm) {
            if(CalcSh(imm)) {
                imm >>= ImmShift;
            }
            return static_cast<u16>(imm);
        }
    };
};

#include "add_immediate.hpp"
#include "adds_immediate.hpp"
#include "sub_immediate.hpp"
#include "subs_immediate.hpp"

/* Alias. */
#include "cmn_immediate.hpp"
#include "cmp_immediate.hpp"