#pragma once

#include "common.hpp"
#include "inline_impl.hpp"

namespace exl::hook::nx64 {

    void Initialize();

    uintptr_t Hook(uintptr_t hook, uintptr_t callback, bool do_trampoline = false);
    void HookInline(uintptr_t hook, uintptr_t callback);
}