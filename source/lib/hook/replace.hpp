#pragma once

#include "base.hpp"
#include "util/func_ptrs.hpp"

#define HOOK_DEFINE_REPLACE(name)                        \
struct name : public ::exl::hook::impl::ReplaceHook<name>

namespace exl::hook::impl {

    template<typename Derived>
    struct ReplaceHook {

        template<typename T = Derived>
        using CallbackFuncPtr = decltype(&T::Callback);

        static ALWAYS_INLINE void InstallAtOffset(ptrdiff_t address) {
            _HOOK_STATIC_CALLBACK_ASSERT();

            hook::Hook(util::modules::GetTargetStart() + address, Derived::Callback);
        }

        template<typename R, typename ...A>
        static ALWAYS_INLINE void InstallAtFuncPtr(util::GenericFuncPtr<R, A...> ptr) {
            _HOOK_STATIC_CALLBACK_ASSERT();

            using ArgFuncPtr = decltype(ptr);
            static_assert(std::is_same_v<ArgFuncPtr, CallbackFuncPtr<>>, "Argument pointer type must match callback type!");

            hook::Hook(ptr, Derived::Callback);
        }

        static ALWAYS_INLINE void InstallAtPtr(uintptr_t ptr) {
            _HOOK_STATIC_CALLBACK_ASSERT();
            
            hook::Hook(ptr, Derived::Callback);
        }
    };

}