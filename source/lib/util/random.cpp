#include "random.hpp"

#include <random>

namespace exl::util {

    u64 GetRandomU64() {
        std::mt19937_64 random { svcGetSystemTick() };
        return random();
    }

    extern "C" u64 exl_random() {
        return GetRandomU64();
    }
}