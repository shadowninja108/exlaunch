#pragma once

#include "orr_shifted_register.hpp"

namespace exl::armv8::inst {

    /* Alias. */
    struct MovRegister : public OrrShiftedRegister {

        static constexpr inline auto GetRn(reg::Register rd, reg::Register rm) {
            /* TODO: static_assert(rd.Is64() == rm.Is64() , ""); */
            return rd.Is64() ? reg::None64 : reg::None32;
        }

        constexpr MovRegister(reg::Register rd, reg::Register rm) : OrrShiftedRegister(rd, GetRn(rd, rm), rm) {}

    };
    
    static_assert(MovRegister(reg::X0, reg::X1).Value() == 0xAA0103E0, "");
    static_assert(MovRegister(reg::X2, reg::X3).Value() == 0xAA0303E2, "");
    static_assert(MovRegister(reg::X4, reg::X5).Value() == 0xAA0503E4, "");
    static_assert(MovRegister(reg::X6, reg::X7).Value() == 0xAA0703E6, "");
}