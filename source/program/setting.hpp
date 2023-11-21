#pragma once

#include "common.hpp"

namespace exl::setting {
    /* How large the fake .bss heap will be. */
    constexpr size_t HeapSize = EXL_FAKEHEAP_SIZE;

    /* How large the JIT area will be for hooks. */
    constexpr size_t JitSize = EXL_JIT_SIZE;

    /* How large the area will be inline hook pool. */
    constexpr size_t InlinePoolSize = EXL_INLINE_POOL_SIZE;

    /* Sanity checks. */
    static_assert(ALIGN_UP(JitSize, PAGE_SIZE) == JitSize, "");
    static_assert(ALIGN_UP(InlinePoolSize, PAGE_SIZE) == JitSize, "");
}