#pragma once

#include "deprecated.hpp"

#define _HOOK_CLASS_VALUE_SUFFIX ClassValue
#define _HOOK_CLASS_SUFFIX Class

#define _MAKE_CLASS_VALUE_NAME(name)  \
    APPEND(name, _HOOK_CLASS_VALUE_SUFFIX)

#define _MAKE_CLASS_NAME(name)  \
    APPEND(name, _HOOK_CLASS_SUFFIX)

#define _MAKE_CLASS_WRAP(type, name)                                    \
    using _MAKE_CLASS_VALUE_NAME(name) = type;                          \
    class _MAKE_CLASS_NAME(name) : public _MAKE_CLASS_VALUE_NAME(name) {

#define MAKE_CLASS_HOOK(ret, type, name, args, body)    \
    _MAKE_CLASS_WRAP(type, name)                        \
        public:                                         \
        _MAKE_HOOK_IMPL(ret, name, args, body)          \
    }


#define MAKE_CLASS_HOOK_T(ret, type, name, args, body)  \
    _MAKE_CLASS_WRAP(type, name)                        \
        public:                                         \
        using FuncValue = ret (type::*) args;           \
        inline static FuncValue _HOOK_IMPL_NAME(name);  \
        _MAKE_HOOK_T_IMPL(ret, name, args, body)        \
    }

#define _HOOK_CLASS_HOOK_NAME(name)  \
    _MAKE_CLASS_NAME(name)::_HOOK_NAME(name)

#define _HOOK_CLASS_FUNC(name, funcname)    \
    _MAKE_CLASS_VALUE_NAME(name)::funcname

#define _HOOK_CLASS_T_NAME(name)  \
    _MAKE_CLASS_NAME(name)::_HOOK_IMPL_NAME(name)

#define INJECT_CLASS_HOOK(funcname, name)   \
    hook::Hook(&_HOOK_CLASS_FUNC(name, funcname), &_HOOK_CLASS_HOOK_NAME(name));
#define INJECT_CLASS_HOOK_T(funcname, name) \
    _HOOK_CLASS_T_NAME(name) = hook::Hook(&_HOOK_CLASS_FUNC(name, funcname), &_HOOK_CLASS_HOOK_NAME(name), true);

#define CPP_IMPL(...)  \
    (this->*impl)(__VA_ARGS__)
