#include "random.hpp"

#include <random>

namespace exl::util {

    namespace {
        std::mt19937_64 s_Rng { svcGetSystemTick() };
    }

    u64 GetRandomU64() {
        return s_Rng();
    }

    extern "C" u64 exl_random() {
        return GetRandomU64();
    }
}