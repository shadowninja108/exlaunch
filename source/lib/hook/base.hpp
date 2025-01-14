#pragma once

#include "util/func_ptrs.hpp"
#include "util/type_traits.hpp"
#include "util/modules.hpp"

#include <type_traits>
#include <cstring>
#include <lib/log/logger_mgr.hpp>
#include <program/loggers.hpp>

#include "nx64/impl.hpp"

#define _HOOK_STATIC_CALLBACK_ASSERT() \
    static_assert(!std::is_member_function_pointer_v<CallbackFuncPtr<>>, "Callback method must be static!")

namespace exl::hook {  

    /* TODO: 32-bit. */
    namespace arch = nx64;
    
    namespace {
        using Entrypoint = util::CFuncPtr<void, void*, void*>;
    };

    inline void NORETURN CallTargetEntrypoint(void* x0, void* x1) {
        auto entrypoint = reinterpret_cast<Entrypoint>(util::modules::GetTargetStart());
        entrypoint(x0, x1);
        UNREACHABLE;
    }

    inline void Initialize() {
        arch::Initialize();
    }
    
    template<typename FuncPtr, typename CallbackPtr>
    requires (!std::is_member_function_pointer_v<FuncPtr>) && (!std::is_member_function_pointer_v<CallbackPtr>)
    CallbackPtr Hook(FuncPtr hook, CallbackPtr callback, bool do_trampoline = false) {
        uintptr_t trampoline = arch::Hook(
            std::bit_cast<uintptr_t>(hook),
            std::bit_cast<uintptr_t>(callback),
            do_trampoline
        );

        return std::bit_cast<CallbackPtr>(trampoline);
    }

    /* For member function pointers. */
    template<typename MemberFuncPtr>
    requires std::is_member_function_pointer_v<MemberFuncPtr> && util::FuncPtrTraits<MemberFuncPtr>::IsMemberFunc
    auto Hook(MemberFuncPtr hook, typename util::FuncPtrTraits<MemberFuncPtr>::CPtr callback, bool do_trampoline = false) {
        auto adapted = util::member_func::Adapt(hook);
        if(adapted.IsVirtual()) {
            R_ABORT_UNLESS(result::VirtualMemberFunctionPointerNotSupported);
        }

        return Hook(adapted.GetPtr(nullptr), callback, do_trampoline);
    }

    using InlineCtx = arch::InlineCtx;
    using InlineCallback = util::CFuncPtr<void, InlineCtx*>;

    using InlineFloatCtx = arch::InlineFloatCtx;
    using InlineFloatCallback = util::CFuncPtr<void, InlineFloatCtx*>;

    inline void HookInline(uintptr_t hook, InlineCallback callback) {
        arch::HookInline(hook, reinterpret_cast<uintptr_t>(callback), false);
    }

    inline void HookInline(uintptr_t hook, InlineFloatCallback callback) {
        arch::HookInline(hook, reinterpret_cast<uintptr_t>(callback), true);
    }
}