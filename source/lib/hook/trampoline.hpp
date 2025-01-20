#pragma once

#include "base.hpp"
#include "util/func_ptrs.hpp"
#include <functional>

#ifdef EXL_LOAD_KIND_MODULE
#include "ro.h"
#endif

#define HOOK_DEFINE_TRAMPOLINE(name)                        \
struct name : public ::exl::hook::impl::TrampolineHook<name>

namespace exl::hook::impl {

    template<typename Derived>
    class TrampolineHook {

        template<typename T = Derived>
        using CallbackFuncPtr = decltype(&T::Callback);

        static ALWAYS_INLINE auto& OrigRef() {
            _HOOK_STATIC_CALLBACK_ASSERT();

            static constinit CallbackFuncPtr<> s_FnPtr = nullptr;

            return s_FnPtr;
        }

        public:
        template<typename... Args>
        static ALWAYS_INLINE decltype(auto) Orig(Args &&... args) {
            _HOOK_STATIC_CALLBACK_ASSERT();

            return OrigRef()(std::forward<Args>(args)...);
        }

        template<typename T>
        static ALWAYS_INLINE void InstallAtFuncPtr(T ptr) {
            _HOOK_STATIC_CALLBACK_ASSERT();

            using Traits = util::FuncPtrTraits<T>;
            static_assert(std::is_same_v<typename Traits::CPtr, CallbackFuncPtr<>>, "Argument pointer type must match callback type!");

            OrigRef() = hook::Hook(ptr, Derived::Callback, true);
        }

        static ALWAYS_INLINE void InstallAtPtr(uintptr_t ptr) {
            _HOOK_STATIC_CALLBACK_ASSERT();
            
            OrigRef() = hook::Hook(ptr, Derived::Callback, true);
        }

#ifdef EXL_LOAD_KIND_MODULE
        static ALWAYS_INLINE void InstallAtSymbol(const char* symbol) {
            _HOOK_STATIC_CALLBACK_ASSERT();

            uintptr_t address = 0;
            EXL_ASSERT(R_SUCCEEDED(nn::ro::LookupSymbol(&address, symbol)), "Symbol not found!");

            OrigRef() = hook::Hook(address, Derived::Callback, true);
        }
#endif
    };

}
