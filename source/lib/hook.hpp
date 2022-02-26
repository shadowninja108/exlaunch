#pragma once

#include "common.hpp"
#include "util/mem_layout.hpp"
#include "util/nx_hook.hpp"
#include "util/typed_storage.hpp"

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
    exl::hook::HookFunc(offset, _HOOK_NAME(name));
#define INJECT_HOOK_T(offset, name) \
    _HOOK_IMPL_NAME(name) = exl::hook::HookFunc(offset, _HOOK_NAME(name), true);

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
    exl::util::Hook::HookFunc(&_HOOK_CLASS_FUNC(name, funcname), &_HOOK_CLASS_HOOK_NAME(name));
#define INJECT_CLASS_HOOK_T(funcname, name) \
    _HOOK_CLASS_T_NAME(name) = exl::util::Hook::HookFunc(&_HOOK_CLASS_FUNC(name, funcname), &_HOOK_CLASS_HOOK_NAME(name), true);

#define CPP_IMPL(...)  \
    (this->*impl)(__VA_ARGS__)

namespace exl::hook {

    namespace impl {
        using entrypoint_t = void (*)(void*,void*);
    };

    inline uintptr_t GetSelfStart() {
        return util::GetSelfModuleInfo().m_Total.m_Start;
    }
    inline uintptr_t GetTargetOffset(uintptr_t offset) {
        return util::GetMainModuleInfo().m_Total.m_Start + offset;
    }
    inline uintptr_t GetTargetStart() {
        return GetTargetOffset(0);
    }

    inline void Initialize() {
        util::Hook::Initialize();
    } 

    inline void NORETURN CallTargetEntrypoint(void* x0, void* x1) {
        auto entrypoint = (impl::entrypoint_t) GetTargetStart();
        entrypoint(x0, x1);
        UNREACHABLE;
    }

    template<typename T>
    inline T HookFunc(T hook, T callback, bool do_trampoline = false) {
        return util::Hook::HookFunc<T>(hook, callback, do_trampoline);
    }

    template<typename T>
    inline T HookFunc(uintptr_t hook, T callback, bool do_trampoline = false) {
        return util::Hook::HookFunc<T>(GetTargetOffset(hook), callback, do_trampoline);
    }

    template<typename T>
    inline T HookFunc(uintptr_t hook, uintptr_t callback, bool do_trampoline = false) {
        return util::Hook::HookFunc<T>(GetTargetOffset(hook), GetTargetOffset(callback), do_trampoline);
    }

    /* Core pointer path following. */
    template<uintptr_t... Offsets>
    inline uintptr_t FollowPointerPathSafe(uintptr_t ptr) {
        uintptr_t curOffset = ptr;

        static constexpr auto offsets = std::array{Offsets...};

        /* Nothing to do if there's no arguments. */
        if constexpr (offsets.size() == 0)
            return curOffset;

        /* Null ptr should just be returned. */
        if(curOffset == 0)
            return 0;

        /* Skip dereferencing the first argument itself. */
        curOffset += offsets[0];
        /* Iterate through rest of offsets. */
        for(size_t i = 1; i < offsets.size(); i++) 
        {
            curOffset = *(uintptr_t*)(curOffset);

            /* Check we can keep following. */
            if(curOffset == 0)
                return 0;

            curOffset += offsets[i];
        }
        return curOffset;
    }
    template<uintptr_t... Offsets>
    inline uintptr_t FollowPointerPath(uintptr_t ptr) {
        uintptr_t curOffset = ptr;

        static constexpr auto offsets = std::array{Offsets...};

        /* Nothing to do if there's no arguments. */
        if constexpr (offsets.size() == 0)
            return curOffset;
            
        /* Null ptr should just be returned. */
        if(curOffset == 0)
            return 0;

        /* Skip dereferencing the first argument itself. */
        curOffset += offsets[0];
        /* Iterate through rest of offsets. */
        for(size_t i = 1; i < offsets.size(); i++) 
        {
            curOffset = *(uintptr_t*)(curOffset);
            curOffset += offsets[i];
        }
        return curOffset;
    }

    /* Pointer helpers. */
    template<uintptr_t... Offsets>
    inline uintptr_t FollowPointerPathSafe(void const* ptr) {
        return FollowPointerPathSafe<Offsets...>(reinterpret_cast<uintptr_t>(ptr));
    }
    template<uintptr_t... Offsets>
    inline uintptr_t FollowPointerPath(void const* ptr) {
        return FollowPointerPath<Offsets...>(reinterpret_cast<uintptr_t>(ptr));
    }

    /* Target relative helpers. */
    template<uintptr_t... Offsets>
    inline uintptr_t FollowPointerPathSafe() {
        return FollowPointerPathSafe<Offsets...>(GetTargetStart());
    }
    template<uintptr_t... Offsets>
    inline uintptr_t FollowPointerPath() {
        return FollowPointerPath<Offsets...>(GetTargetStart());
    }

    /* Casting helpers.  */
    template<typename EndType, uintptr_t... Offsets>
    inline EndType* FollowPointerPathSafe() {
        auto offset = FollowPointerPathSafe<Offsets...>();
        return (EndType*) offset;
    }
    template<typename EndType, uintptr_t... Offsets>
    inline EndType FollowPointerPath() {
        auto offset = FollowPointerPath<Offsets...>();
        return *(EndType*) offset;
    }
    template<typename EndType, uintptr_t... Offsets>
    inline EndType* FollowPointerPathSafe(uintptr_t ptr) {
        auto offset = FollowPointerPathSafe<Offsets...>(ptr);
        return (EndType*) offset;
    }
    template<typename EndType, uintptr_t... Offsets>
    inline EndType FollowPointerPath(uintptr_t ptr) {
        auto offset = FollowPointerPath<Offsets...>(ptr);
        return *(EndType*) offset;
    }
    template<typename EndType, uintptr_t... Offsets>
    inline EndType* FollowPointerPathSafe(void const* ptr) {
        auto offset = FollowPointerPathSafe<Offsets...>(ptr);
        return (EndType*) offset;
    }
    template<typename EndType, uintptr_t... Offsets>
    inline EndType FollowPointerPath(void const* ptr) {
        auto offset = FollowPointerPath<Offsets...>(ptr);
        return *(EndType*) offset;
    }

}