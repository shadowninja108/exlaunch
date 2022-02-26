#pragma once

#include <lib/armv8.hpp>

namespace exl::armv8::inst {

    struct Instruction : public InstBitSet {

        ACCESSOR(MainOp0, 25, 29);

        constexpr Instruction(u8 op0) {
            SetMainOp0(op0);
        }
    };

}

#include "op100x/base.hpp"
#include "op101x/base.hpp"
#include "opx101/base.hpp"