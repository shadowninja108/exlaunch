#pragma once

#include <common.hpp>
#include <utility>

#include "table.hpp"

namespace exl::reloc {
    template<typename VersionType, typename... VersionedTables>
    struct TableSet {
        static_assert((true && ... && std::is_same_v<VersionType, typename VersionedTables::VersionType>), "Version type within table array does not match version of set!");

        using TablesTuple = std::tuple<VersionedTables...>;
        static constexpr size_t s_Count = sizeof...(VersionedTables);
        static constexpr std::array<VersionType, s_Count> s_Versions { VersionedTables::s_Version... };

        static constexpr TablesTuple s_Map {};

        constexpr size_t FindTableIndex(VersionType ver) const {
            /* Check if the user provided multiple tables for the same version. */
            static_assert(impl::IsUnique(s_Versions), "Duplicate version table!");

            auto found = std::find(s_Versions.begin(), s_Versions.end(), ver);
            if(found == s_Versions.end())
                return SIZE_MAX;

            /* TODO: this kinda sucks to get the index... */
            return found - s_Versions.begin();
        }

        template<size_t... Indicies>
        ALWAYS_INLINE TableBin GetTableByIndexImpl(size_t index, std::index_sequence<Indicies...>) const {
            /* Check if any of the entries in the table have the same symbol hash. This can occur if there happens to be a collision. */
            static_assert(
                (true && ... && impl::IsUnique(std::get<Indicies>(s_Map).m_Table, [](const LookupEntryBin& v) { return v.m_SymbolHash; })),
                "Symbol name hash collision!"
            );
            /* Create array of spans to access tables at runtime by index. */
            static constexpr auto tables = std::array<TableBin, s_Count> {
                (TableBin(
                    std::get<Indicies>(s_Map).m_Table.data(),
                    std::get<Indicies>(s_Map).m_Table.size()
                ))...
            };
            
            /* Ensure the index isn't out of bounds... */
            EXL_ABORT_UNLESS(0 <= index && index < s_Count);

            return tables[index];

        }

        inline TableBin GetTableByIndex(size_t index) const {
            return GetTableByIndexImpl(index, std::make_index_sequence<s_Count>{});
        }

        public:
        inline bool DoesTableExist(VersionType type) const {
            return FindTableIndex(type) != SIZE_MAX;
        }
        ALWAYS_INLINE TableBin Get(VersionType type) const {
            auto index = FindTableIndex(type);
            EXL_ABORT_UNLESS(index != SIZE_MAX);
            return GetTableByIndex(index);
        }
    };
}