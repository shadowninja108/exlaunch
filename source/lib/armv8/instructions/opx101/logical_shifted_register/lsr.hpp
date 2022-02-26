#pragma once

#include "orr_shifted_register.hpp"

namespace exl::armv8::inst {

    struct Lsr : public OrrShiftedRegister {

        constexpr Lsr(reg::Register rd, reg::Register rn, reg::Register rm, u16 amount) 
        : OrrShiftedRegister(rd, rn, rm, LSR, amount) {}
    };
}