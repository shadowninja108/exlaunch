#pragma once

#include <common.hpp>

namespace exl::util {
    
    struct Range {
        uintptr_t m_Start;
        size_t m_Size;

        constexpr uintptr_t GetEnd() const { return m_Start + m_Size; }

        constexpr bool InRange(uintptr_t value) const {
            return m_Start <= value && value < GetEnd();
        }

        constexpr bool InRangeInclusive(uintptr_t value) const {
            return m_Start <= value && value <= GetEnd();
        }
    };
}