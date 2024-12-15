#pragma once

#include <string_view>
#include <cstdint>
#include <climits>
#include <lib/util/murmur3.hpp>
#include <lib/util/module_index.hpp>

namespace exl::reloc {

    using HashType = util::Murmur3::BlockType;

    struct LookupEntryBin {
        HashType m_SymbolHash;

        /* Leave upper bits free for the module index. */
        uint32_t m_Offset : ((sizeof(uint32_t) * CHAR_BIT) - util::s_ModuleIndexBitCount);
        /* Fit the module index into the upper bits. */
        util::ModuleIndex m_ModuleIndex : util::s_ModuleIndexBitCount;
        /* Can never be too sure... */
        static_assert(CHAR_BIT == 8);

        constexpr bool operator<(const LookupEntryBin& other) const {
            return m_SymbolHash < other.m_SymbolHash;
        }

        constexpr bool operator<(const HashType& other) const {
            return m_SymbolHash < other;
        }
    };

    namespace impl {

        /* Would've liked to have used std::string_view here, but implementation demands this type be "non-structural", so this is how it has to be. */
        template<size_t StringSize>
        struct LookupEntry {
            util::ModuleIndex m_ModuleIndex;
            uint32_t m_Offset;
            char m_Symbol[StringSize];

            constexpr std::string_view GetSymbol() const {
                return std::string_view(m_Symbol, StringSize-1);
            }

            constexpr LookupEntryBin Convert() const {
                return LookupEntryBin(
                    util::Murmur3::Compute(GetSymbol()),
                    m_Offset,
                    m_ModuleIndex
                );
            }
        };
    }
}