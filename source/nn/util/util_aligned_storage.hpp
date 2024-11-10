#pragma once

#include <cstdint>
#include <cstddef>

namespace nn::util {

    template<std::size_t Size, std::size_t Align>
    struct AlignedStorage {
        struct Type {
            alignas(Align) std::byte data[Size];
        };
    };

}