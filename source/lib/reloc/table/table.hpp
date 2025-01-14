#pragma once

#include <span>
#include <utility>

#include "lookup_entry.hpp"

namespace exl::reloc {

    namespace impl {
        template<typename In, typename Delegate>
        constexpr bool IsUnique(In lookups, Delegate delegate) {
            const auto view = lookups | std::views::transform(delegate);
            return std::ranges::adjacent_find(view) == view.end();
        }
        template<typename In>
        constexpr bool IsUnique(In lookups) {
            return IsUnique(lookups, [](const auto& v) {
                return v;
            });
        }

        static constexpr const auto Sort(const auto input) {
            auto sorted = input;
            std::sort(sorted.begin(), sorted.end());
            return sorted;
        }
    }

    template<impl::LookupEntry... Table>
    struct TableType {
        static constexpr size_t s_Size = sizeof...(Table);

        using Array = std::array<LookupEntryBin, s_Size>;

        static constexpr Array s_UnsortedTable { Table.Convert()... };
        static constexpr Array s_SortedTable = impl::Sort(s_UnsortedTable);

        /* Ensure the symbol strings are unique. */
        static_assert(impl::IsUnique(std::array<std::string_view, s_Size> {Table.GetSymbol()...}), "Duplicate symbol!");

        Array m_Table = s_SortedTable;
    };
    template<auto Version, impl::LookupEntry... Table>
    struct VersionedTable : public TableType<Table...> {
        using VersionType = decltype(Version);
        static constexpr auto s_Version = Version;
    };
}