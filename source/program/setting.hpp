#pragma once

#include "common.hpp"

#ifdef VERSION_SHIELD
#define EXL_MODULE_NAME "exlaunch_shield"
#define EXL_MODULE_NAME_LEN 15
#else
#define EXL_MODULE_NAME "exlaunch_sword"
#define EXL_MODULE_NAME_LEN 14
#endif

#define EXL_DEBUG
#define EXL_USE_FAKEHEAP

/*
#define EXL_SUPPORTS_REBOOTPAYLOAD
*/

namespace exl::setting {
    /* How large the fake .bss heap will be. */
    constexpr size_t HeapSize = 0x5000;

    /* How large the JIT area will be for hooks. */
    constexpr size_t JitSize = 0x1000;

    /* How large the area will be inline hook pool. */
    constexpr size_t InlinePoolSize = 0x1000;

    /* Sanity checks. */
    static_assert(ALIGN_UP(JitSize, PAGE_SIZE) == JitSize, "");
    static_assert(ALIGN_UP(InlinePoolSize, PAGE_SIZE) == JitSize, "");
}