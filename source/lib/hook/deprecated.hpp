#pragma once

#define _IMPL_SUFFIX Impl
#define _HOOK_SUFFIX Hook

#define _HOOK_IMPL_NAME(name)   \
    APPEND(name, _IMPL_SUFFIX)
#define _HOOK_NAME(name)    \
    APPEND(name, _HOOK_SUFFIX)

#define _MAKE_HOOK_IMPL(ret, name, args, body)  \
    ret _HOOK_NAME(name) args {                 \
        body                                    \
    }

#define MAKE_HOOK(ret, name, args, body)    \
    static _MAKE_HOOK_IMPL(ret, name, args, body)

#define _MAKE_HOOK_T_IMPL(ret, name, args, body)    \
    ret _HOOK_NAME(name) args {                     \
        const auto impl = _HOOK_IMPL_NAME(name);    \
        body                                        \
    }

#define MAKE_HOOK_T(ret, name, args, body)          \
    static ret (*_HOOK_IMPL_NAME(name)) args;       \
    static _MAKE_HOOK_T_IMPL(ret, name, args, body)
    
#define INJECT_HOOK(offset, name)   \
    exl::hook::Hook(offset, _HOOK_NAME(name));
#define INJECT_HOOK_T(offset, name) \
    _HOOK_IMPL_NAME(name) = exl::hook::Hook(offset, _HOOK_NAME(name), true);
