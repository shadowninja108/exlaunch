#pragma once

#include <common.hpp>

#include "base.hpp"

#define HOOK_DEFINE_INLINE(name)                        \
struct name : public ::exl::hook::impl::InlineHook<name>

namespace exl::hook::impl {

    template<typename Derived>
    struct InlineHook {
        
        template<typename T = Derived>
        using CallbackFuncPtr = decltype(&T::Callback);

        static ALWAYS_INLINE void InstallAtOffset(ptrdiff_t address) {
            _HOOK_STATIC_CALLBACK_ASSERT();

            hook::HookInline(util::modules::GetTargetStart() + address, Derived::Callback);
        }

        static ALWAYS_INLINE void InstallAtPtr(uintptr_t ptr) {
            _HOOK_STATIC_CALLBACK_ASSERT();
            
            hook::HookInline(ptr, Derived::Callback);
        }

    };
}