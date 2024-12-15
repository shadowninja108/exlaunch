#include "reloc.hpp"

#include <lib/util/version.hpp>
#include <lib/log/logger_mgr.hpp>
#include <program/loggers.hpp>

#include "table/table_gen.hpp"

namespace exl::reloc {

    namespace impl {

        static bool s_InitializeSucceeded = false;
        Lookup s_CachedLookup;
        
        void Initialize() {
            /* If the user has no tables, they aren't trying to use this feature. Therefore, there's nothing to do and it's a success. */
            if constexpr(impl::s_TableCount == 0) {
                s_InitializeSucceeded = true;
                return;
            }
            
            auto version = util::GetUserVersion();
            auto index = impl::FindTableIndex(version);
            if(index == SIZE_MAX) {
                Logging.Log(EXL_LOG_PREFIX "Failed to find lookup table for version 0x%x. Ignoring...", static_cast<int>(version));
                s_InitializeSucceeded = false;
                return;
            }
            
            s_InitializeSucceeded = true;
            s_CachedLookup.m_Entries = impl::GetTableByIndex(index);
            s_CachedLookup.Apply();
        }
    }
}