#pragma once

#include "base.hpp"

namespace exl::armv8::inst {

    struct Nop : public impl::op101x::Hints {

        static constexpr u8 CRm = 0b0000;
        static constexpr u8 LocalOp2 = 0b000;

        constexpr Nop() : Hints() {
            SetCRm(CRm);
            SetLocalOp2(LocalOp2);
        }
    };

    static_assert(Nop().Value() == 0xD503201F, "");
}