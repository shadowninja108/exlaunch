#pragma once

#include "nx64/nx_hook.hpp"
#include "util/func_ptrs.hpp"
#include "util/modules.hpp"

#define _HOOK_STATIC_CALLBACK_ASSERT() \
    static_assert(!std::is_member_function_pointer_v<CallbackFuncPtr<>>, "Callback method must be static!")

namespace exl::hook {    
    
    namespace {
        using Entrypoint = util::GenericFuncPtr<void, void*, void*>;
    };

    inline void NORETURN CallTargetEntrypoint(void* x0, void* x1) {
        auto entrypoint = reinterpret_cast<Entrypoint>(util::modules::GetTargetStart());
        entrypoint(x0, x1);
        UNREACHABLE;
    }

    inline void Initialize() {
        nx64::Initialize();
    } 

    template<typename T>
    inline T HookFunc(T hook, T callback, bool do_trampoline = false) {
        return nx64::HookFunc<T>(hook, callback, do_trampoline);
    }

    template<typename T>
    inline T HookFunc(uintptr_t hook, T callback, bool do_trampoline = false) {
        return nx64::HookFunc<T>(util::modules::GetTargetOffset(hook), callback, do_trampoline);
    }

    template<typename T>
    inline T HookFunc(uintptr_t hook, uintptr_t callback, bool do_trampoline = false) {
        return nx64::HookFunc<T>(util::modules::GetTargetOffset(hook), util::modules::GetTargetOffset(callback), do_trampoline);
    }
    
}