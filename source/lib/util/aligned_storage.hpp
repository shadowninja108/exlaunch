#pragma once

#include <cstdint>

namespace exl::util {

    template<std::size_t Size, std::size_t Align>
    struct AlignedStorage {
        struct Type {
            alignas(Align) std::byte data[Size];
        };
    };

}