#pragma once

#include <lib/armv8.hpp>

namespace exl::armv8::inst::impl::op100x {

    struct LogicalImmediate : public Op100xInstruction {

        static constexpr u8 Op0 = 0b100;

        ACCESSOR(Sf,    31);
        ACCESSOR(Opc,   29, 31);
        ACCESSOR(N,     22);
        ACCESSOR(Immr,  16, 22);
        ACCESSOR(Imms,  10, 16);
        ACCESSOR(Rn,    5, 10);
        ACCESSOR(Rd,    0, 5);

        constexpr LogicalImmediate(u8 sf, u8 opc) : Op100xInstruction(Op0) {
            SetSf(sf);
            SetOpc(opc);
        }
    };
};

/* TODO: make this work well... */
// #include "orr_immediate.hpp"
