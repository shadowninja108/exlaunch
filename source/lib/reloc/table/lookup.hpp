#pragma once

#include <common.hpp>
#include <span>
#include <string_view>
#include "lookup_entry.hpp"

namespace exl::reloc {
    
    /* TODO: organize this to be a bit more flexible... */
    struct Lookup {
        NON_COPYABLE(Lookup);
        NON_MOVEABLE(Lookup);

        std::span<const LookupEntryBin> m_Entries;

        Lookup() = default;

        ALWAYS_INLINE const auto& GetEntries() const {
            return m_Entries;
        }

        inline constexpr const LookupEntryBin* FindByHash(HashType hash) const {
            auto low = std::lower_bound(m_Entries.begin(), m_Entries.end(), hash);

            if(low->m_SymbolHash != hash)
                return nullptr;

            return low.base();
        }

        inline const LookupEntryBin* FindByName(std::string_view string) const {
            auto hash = util::Murmur3::Compute(std::span { string.data(), string.size() });
            return FindByHash(hash);
        }
        
        void Apply() const;
    };
}