#pragma once

#include <common.hpp>

namespace exl::util {

    u64 GetRandomU64();

    /* TODO: remove along with libnx. */
    extern "C" u64 exl_random();
}