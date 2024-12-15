#pragma once

#include <common.hpp>
#include <tuple>

namespace exl::reloc {
    using UserVersionMap = std::tuple<
        /*
        // This feature allows you to specify symbols in your executable to resolve with module+offset pairs.
        // They are packed up and sorted at compile time so they can be efficiently looked up.
        // The `exl::reloc::GetLookupTable` API is provided if you want to look up entries in the table explicitly.
        // Examples of tables:
        TableType<VersionType::DEFAULT,
        //    Module offset is relative to.     Offset within module.       Symbol name.
            { util::ModuleIndex::Main,          0x6961,                     "example1" },
            { util::ModuleIndex::Sdk,           0x6962,                     "example2" },
            { util::ModuleIndex::Rtld,          0x6963,                     "example3" }
        >,

        // In addition, you can specify multiple tables and select the correct one at runtime. This allows you to
        // support multiple versions/variations of a game in one executable. See version.hpp to see how to implement
        // multiple supported versions.
        TableType<VersionType::OTHER,
        //    Module offset is relative to.     Offset within module.       Symbol name.
            { util::ModuleIndex::Main,          0x4201,                     "example1" },
            { util::ModuleIndex::Sdk,           0x4202,                     "example2" }
        >
        */
    >;
}