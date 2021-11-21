#pragma once

#include "types.h"

namespace exl::setting {

    enum class LoadKind {
        Kip,
        AsRtld,
        Module,
    };

    constexpr LoadKind SelfLoadKind = LoadKind::EXL_LOAD_KIND;
    constexpr u64 ProgramId = EXL_PROGRAM_ID;
};