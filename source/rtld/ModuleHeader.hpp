#pragma once

#include <stdint.h>

const uint32_t MOD0_MAGIC = 0x30444F4D;

namespace rtld {
    struct ModuleHeader {
        uint32_t magic;
        uint32_t dynamic_offset;
        uint32_t bss_start_offset;
        uint32_t bss_end_offset;
        uint32_t unwind_start_offset;
        uint32_t unwind_end_offset;
        uint32_t module_object_offset;
    };
}