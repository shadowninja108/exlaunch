#pragma once

#include <lib/armv8.hpp>

namespace exl::armv8::inst::impl::op100x {

    struct PcRelAddressing : public Op100xInstruction {

        static constexpr u8 Op0 = 0b000;

        ACCESSOR(Op,    31);
        ACCESSOR(Immlo, 29, 31);
        ACCESSOR(Immhi, 5, 24);
        ACCESSOR(Rd,    0, 5);

        enum Op : u8 {
            Op_ADR  = 0,
            Op_ADRP = 1,
        };

        constexpr PcRelAddressing(reg::Register reg, u32 imm, Op op) : Op100xInstruction(Op0) {
            /*
                static_assert(reg.Is64()); 
            */
            SetOp(op);
            SetImmlo(imm);
            SetImmhi(imm >> ImmloMask.Count);
            SetRd(reg.Index());
        }
    };
};

#include "adr.hpp"
#include "adrp.hpp"