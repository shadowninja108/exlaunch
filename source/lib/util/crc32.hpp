#pragma once

#include <array>
#include <type_traits>
#include <cstddef>
#include <numeric>

namespace exl::util {
    /* TODO: Probably support other CRC sizes? Maybe an API change? */
    namespace impl::crc32 {
        template<typename T>
        concept Hashable = 
            std::is_same_v<T, std::byte> || 
            std::is_same_v<T, char> || 
            std::is_same_v<T, unsigned char>;

        using BlockType = unsigned int;
        using Table = std::array<BlockType, 0x100>;

        static constexpr BlockType s_DefaultPolynomial = 0xEDB88320;

        static constexpr Table CreateTable(BlockType polynomial) {
            Table table;
            /* Fill table with 0, 1, 2, etc. */
            std::iota(table.begin(), table.end(), 0);

            for(size_t i = 0; i < table.size(); i++) {
                auto& item = table[i];
                for(size_t bit = 0; bit < 8; bit++) {
                    if(item & 1) {
                        item  = polynomial ^ (item >> 1);
                    } else {
                        item >>= 1;
                    }
                }
            }

            return table;
        }
    }

    class Crc32 {
        using BlockType = impl::crc32::BlockType;
        using Table = impl::crc32::Table;

        static constexpr Table s_Table = impl::crc32::CreateTable(impl::crc32::s_DefaultPolynomial);

        public:

        template<typename T, size_t Size>
        requires impl::crc32::Hashable<T>
        static constexpr inline BlockType Hash(std::span<const T, Size> data, BlockType init = -1) {
            BlockType ctx = init;
            for(const auto& e : data) {
                auto byte = static_cast<std::byte>(e);
                ctx = s_Table[static_cast<int>(byte ^ static_cast<std::byte>(ctx & 0xFF))] ^ (ctx >> 8);
            }
            return ~ctx;
        }

        /* TODO: Non-constexpr implementation using ARM crypto extensions. */

        static constexpr inline BlockType Hash(std::string_view sv, BlockType init = -1) {
            return Hash(std::span<const char>(sv.data(), sv.size()), init);
        }
    };
}