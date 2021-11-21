#pragma once

#include "abort.hpp"

namespace exl::diag {
    void NORETURN AbortToPayload(const AbortCtx&);
};