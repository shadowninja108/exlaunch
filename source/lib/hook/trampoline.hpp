#pragma once

#include "base.hpp"
#include "util/func_ptrs.hpp"
#include "reloc/reloc.hpp"
#include <functional>

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

        static ALWAYS_INLINE void InstallAtOffset(ptrdiff_t address) {
            _HOOK_STATIC_CALLBACK_ASSERT();

            OrigRef() = hook::Hook(util::modules::GetTargetStart() + address, Derived::Callback, true);
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

        static ALWAYS_INLINE void InstallAtSymbol(const char* symbol) {
            _HOOK_STATIC_CALLBACK_ASSERT();

            const exl::reloc::LookupEntryBin* entry = exl::reloc::GetLookupTable().FindByName(symbol);
            EXL_ASSERT(entry, "Symbol not found!");

            OrigRef() = hook::Hook(util::modules::GetTargetOffset(entry->m_Offset), Derived::Callback, true);
        }
    };

}
