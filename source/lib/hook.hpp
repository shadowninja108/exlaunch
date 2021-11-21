#pragma once

#include "common.hpp"
#include "util/nx_hook.hpp"

#define APPEND_IMPL(x, y) x ## y
#define APPEND(x, y) APPEND_IMPL(x, y)

#define _IMPL_SUFFIX Impl
#define _HOOK_SUFFIX Hook

#define _HOOK_IMPL_NAME(name)   \
    APPEND(name, _IMPL_SUFFIX)
#define _HOOK_NAME(name)    \
    APPEND(name, _HOOK_SUFFIX)

#define MAKE_HOOK(ret, name, args, body)    \
    ret _HOOK_NAME(name) args {             \
        body                                \
    }

#define MAKE_HOOK_T(ret, name, args, body)          \
    ret (*_HOOK_IMPL_NAME(name)) args;              \
    ret _HOOK_NAME(name) args {                     \
        const auto impl = _HOOK_IMPL_NAME(name);    \
        body                                        \
    }
    
#define INJECT_HOOK(offset, name)   \
    exl::hook::HookFunc(offset, _HOOK_NAME(name));
#define INJECT_HOOK_T(offset, name) \
    _HOOK_IMPL_NAME(name) = exl::hook::HookFunc(offset, _HOOK_NAME(name), true);

namespace exl::hook {

    uintptr_t GetSelfStart();
    uintptr_t GetTargetStart();
    uintptr_t GetTargetOffset(uintptr_t offset);
    NORETURN void CallTargetEntrypoint(void*, void*);

    void Initialize();

    template<typename T>
    T HookFunc(T hook, T callback, bool do_trampoline = false) {
        return util::Hook::HookFunc<T>(hook, callback, do_trampoline);
    }

    template<typename T>
    T HookFunc(uintptr_t hook, T callback, bool do_trampoline = false) {
        return util::Hook::HookFunc<T>(GetTargetOffset(hook), callback, do_trampoline);
    }

    template<typename T>
    T HookFunc(uintptr_t hook, uintptr_t callback, bool do_trampoline = false) {
        return util::Hook::HookFunc<T>(GetTargetOffset(hook), GetTargetOffset(callback), do_trampoline);
    }
};