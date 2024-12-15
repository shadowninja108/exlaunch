#pragma once

#include <span>
#include <tuple>
#include <utility>
#include <program/version.hpp>
#include "lookup.hpp"

namespace exl::reloc {

    /* Easy access to the version type for us and user. */
    using VersionType = util::UserVersion;

    namespace impl {
        
        using TableBin = std::span<const LookupEntryBin>;
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

    template<VersionType _Version, impl::LookupEntry... _Table>
    struct TableType {
        static constexpr VersionType s_Version = _Version;
        static constexpr size_t s_Size = sizeof...(_Table);
        using Array = std::array<LookupEntryBin, s_Size>;

        static constexpr Array s_UnsortedTable { _Table.Convert()... };
        static constexpr Array s_SortedTable = impl::Sort(s_UnsortedTable);

        /* Ensure the symbol strings are unique. */
        static_assert(impl::IsUnique(std::array<std::string_view, s_Size> {_Table.GetSymbol()...}), "Duplicate symbol!");

        Array m_Table = s_SortedTable;
    };
}

#include <program/offsets.hpp>

namespace exl::reloc {

    /* TODO: organize this to be a bit more flexible... */
    namespace impl {

        using Map = UserVersionMap;

        static constexpr Map s_Map{};
        static constexpr size_t s_TableCount = std::tuple_size<Map>{};

        template<size_t... Indicies>
        static constexpr auto ExtractVersions(std::index_sequence<Indicies...>) {
            return std::array<VersionType, sizeof...(Indicies)> { (std::tuple_element_t<Indicies, Map>::s_Version)... };
        }

        constexpr size_t FindTableIndex(VersionType ver) {
            static constexpr std::array<VersionType, s_TableCount> versions = ExtractVersions(std::make_index_sequence<s_TableCount>{});
            /* Check if the user provided multiple tables for the same version. */
            static_assert(IsUnique(versions), "Duplicate version table!");

            auto found = std::find(versions.begin(), versions.end(), ver);
            if(found == versions.end())
                return SIZE_MAX;

            /* TODO: this kinda sucks to get the index... */
            return found - versions.begin();
        }

        template<size_t... Indicies>
        ALWAYS_INLINE TableBin GetTableByIndexImpl(size_t index, std::index_sequence<Indicies...>) {
            /* Check if any of the entries in the table have the same symbol hash. This can occur if there happens to be a collision. */
            static_assert(
                (true && ... && IsUnique(std::get<Indicies>(s_Map).m_Table, [](const LookupEntryBin& v) { return v.m_SymbolHash; })),
                "Symbol name hash collision!"
            );
            /* Create array of spans to access tables at runtime by index. */
            static constexpr auto tables = std::array<TableBin, s_TableCount> {
                (TableBin(
                    std::get<Indicies>(s_Map).m_Table.data(),
                    std::get<Indicies>(s_Map).m_Table.size()
                ))...
            };
            
            /* Ensure the index isn't out of bounds... */
            EXL_ABORT_UNLESS(0 <= index && index < s_TableCount);

            return tables[index];

        }

        inline TableBin GetTableByIndex(size_t index) {
            return GetTableByIndexImpl(index, std::make_index_sequence<s_TableCount>{});
        }
    }
}
