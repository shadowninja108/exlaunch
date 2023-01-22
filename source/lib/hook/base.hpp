#pragma once

#include "util/func_ptrs.hpp"
#include "util/modules.hpp"

#include <type_traits>
#include <cstring>

#include "nx64/impl.hpp"

#define _HOOK_STATIC_CALLBACK_ASSERT() \
    static_assert(!std::is_member_function_pointer_v<CallbackFuncPtr<>>, "Callback method must be static!")

namespace exl::hook {  

    /* TODO: 32-bit. */
    namespace arch = nx64;
    
    namespace {
        using Entrypoint = util::GenericFuncPtr<void, void*, void*>;
    };

    inline void NORETURN CallTargetEntrypoint(void* x0, void* x1) {
        auto entrypoint = reinterpret_cast<Entrypoint>(util::modules::GetTargetStart());
        entrypoint(x0, x1);
        UNREACHABLE;
    }

    inline void Initialize() {
        arch::Initialize();
    }
    
    template<typename InFunc, typename CbFunc>
    CbFunc Hook(InFunc hook, CbFunc callback, bool do_trampoline = false) {

        /* Workaround for being unable to cast member functions. */
        /* Probably some horrible UB here? */
        uintptr_t hookp;
        uintptr_t callbackp;
        std::memcpy(&hookp, &hook, sizeof(hookp));
        std::memcpy(&callbackp, &callback, sizeof(callbackp));

        uintptr_t trampoline = arch::Hook(hookp, callbackp, do_trampoline);

        /* Workaround for being unable to cast member functions. */
        /* Probably some horrible UB here? */
        CbFunc ret;
        std::memcpy(&ret, &trampoline, sizeof(trampoline));

        return ret;
    }

    using InlineCtx = arch::InlineCtx;
    using InlineCallback = void (*)(InlineCtx*);

    inline void HookInline(uintptr_t hook, InlineCallback callback) {
        arch::HookInline(hook, reinterpret_cast<uintptr_t>(callback));
    }
}