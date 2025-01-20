#pragma once

#include "base.hpp"
#include "util/func_ptrs.hpp"
#include "util/type_traits.hpp"

#ifdef EXL_LOAD_KIND_MODULE
#include "ro.h"
#endif

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

        template<typename T>
        static ALWAYS_INLINE void InstallAtFuncPtr(T ptr) {
            _HOOK_STATIC_CALLBACK_ASSERT();

            using Traits = util::FuncPtrTraits<T>;
            static_assert(std::is_same_v<typename Traits::CPtr, CallbackFuncPtr<>>, "Argument pointer type must match callback type!");

            hook::Hook(ptr, Derived::Callback);
        }

        static ALWAYS_INLINE void InstallAtPtr(uintptr_t ptr) {
            _HOOK_STATIC_CALLBACK_ASSERT();
            
            hook::Hook(ptr, Derived::Callback);
        }

#ifdef EXL_LOAD_KIND_MODULE
        static ALWAYS_INLINE void InstallAtSymbol(const char* symbol) {
            _HOOK_STATIC_CALLBACK_ASSERT();

            uintptr_t address = 0;
            EXL_ASSERT(R_SUCCEEDED(nn::ro::LookupSymbol(&address, symbol)), "Symbol not found!");

            hook::Hook(address, Derived::Callback);
        }
#endif
    };
}