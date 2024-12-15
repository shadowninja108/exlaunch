#pragma once

#include <bit>
#include <cstdint>
#include <span>
#include <algorithm>
#include <ranges>

namespace exl::util {

    namespace impl::murmur3 {
        template<typename T>
        concept Hashable = 
            std::same_as<T, std::byte> || 
            std::same_as<T, char>;
    }

    struct Murmur3 {

        using HashType = uint32_t;
        using BlockType = HashType;

        static constexpr uint32_t C1 = 0xcc9e2d51;
        static constexpr uint32_t C2 = 0x1b873593;
        static constexpr uint32_t R1 = 15;
        static constexpr uint32_t R2 = 13;
        static constexpr uint32_t M = 5;
        static constexpr uint32_t N = 0xe6546b64;

        uint32_t m_Length = 0;
        BlockType m_H = 0;

        constexpr static BlockType Scramble(BlockType k) {
            k *= C1;
            k = std::rotl(k, R1);
            k *= C2;
            return k;
        }

        constexpr void Initialize(const uint32_t seed = 0) {
            m_H = seed;
            m_Length = 0;
        }

        constexpr void Update(BlockType block) {
            m_H ^= Scramble(block);
            m_H = std::rotl(m_H, R2);
            m_H = m_H * M + N;
            m_Length += sizeof(BlockType);
        }

        template<typename T, size_t Size>
        requires impl::murmur3::Hashable<T>
        constexpr HashType Finalize(std::span<const T, Size> end)
        {
            BlockType k = 0;

            switch (end.size()) {
            case 3:
                k ^= static_cast<HashType>(end[2] << 16);
                /* fallthrough */
            case 2:
                k ^= static_cast<HashType>(end[1] << 8);
                /* fallthrough */
            case 1:
                k ^= static_cast<HashType>(end[0] << 0);
                m_H ^= Scramble(k);
            }
            m_Length += end.size() % sizeof(BlockType);

            m_H ^= m_Length;

            auto h = m_H;
            h ^= h >> 16;
            h *= 0x85ebca6b;
            h ^= h >> 13;
            h *= 0xc2b2ae35;
            h ^= h >> 16;

            Initialize(0);
            return h;
        }

        template<typename T, size_t Size>
        requires impl::murmur3::Hashable<T>
        constexpr static HashType Compute(std::span<const T, Size> input, BlockType seed = 0) {
            Murmur3 m{};
            m.Initialize(seed);

            const auto size = input.size();
            const auto remainder = size % sizeof(BlockType);
            const auto roundedDown = size - remainder;

            for(size_t i = 0; i < roundedDown; i += sizeof(BlockType)) {
                /* Copy a block size amount of bytes into an array. */
                std::array<T, sizeof(BlockType)> bytes {};
                auto span = input.subspan(i, sizeof(BlockType));
                std::copy(span.begin(), span.end(), bytes.begin());

                /* Hash the block. */
                m.Update(std::bit_cast<BlockType>(bytes));
            }

            /* Hash remainder and finalize, returning the hash. */
            return m.Finalize(input.subspan(roundedDown, remainder));
        }

        static constexpr inline BlockType Compute(std::string_view sv, BlockType seed = 0) {
            return Compute(std::span<const char>(sv.data(), sv.size()), seed);
        }
    };
}