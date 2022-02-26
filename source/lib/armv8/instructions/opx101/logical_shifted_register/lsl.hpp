#pragma once

#include "orr_shifted_register.hpp"

namespace exl::armv8::inst {

    struct Lsl : public OrrShiftedRegister {

        constexpr Lsl(reg::Register rd, reg::Register rn, reg::Register rm, u16 amount) 
        : OrrShiftedRegister(rd, rn, rm, LSL, amount) {}
    };

    /* Alias. */
    struct MovRegister : public Lsl {

        static constexpr inline auto GetRn(reg::Register rd, reg::Register rm) {
            /* TODO: static_assert(rd.Is64() == rm.Is64() , ""); */
            return rd.Is64() ? reg::None64 : reg::None32;
        }

        constexpr MovRegister(reg::Register rd, reg::Register rm) : Lsl(rd, GetRn(rd, rm), rm, 0) {}
    };
}