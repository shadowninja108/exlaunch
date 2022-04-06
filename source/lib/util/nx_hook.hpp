/*
 *  @date   : 2018/04/18
 *  @author : Rprop (r_prop@outlook.com)
 *  https://github.com/Rprop/And64InlineHook
 */
/*
 MIT License

 Copyright (c) 2018 Rprop (r_prop@outlook.com)

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */
#pragma once

#include "common.hpp"

#include <cstring>
#include <type_traits>

namespace exl::util {

    template<typename T>
    concept RealFunction = std::is_function_v<T> || std::is_function_v<std::remove_pointer_t<T>> || std::is_function_v<std::remove_reference_t<T>>;

    class Hook {
        private:
        static Jit s_HookJit;
        static Jit s_InlineHookJit;
        static size_t s_UsedInlineHooks;

        static uintptr_t HookFuncCommon(uintptr_t hook, uintptr_t callback, bool do_trampoline = false);
        static void InlineHook(uintptr_t hook, uintptr_t callback, bool is_extended = false);
        static Result AllocForTrampoline(uint32_t** rx, uint32_t** rw);

        public:

        typedef union {
            u64 x;  ///< 64-bit AArch64 register view.
            u32 w;  ///< 32-bit AArch64 register view.
            u32 r;  ///< AArch32 register view.
        } CpuRegister;

        typedef union {
            u128 q;
            f64  d[2];
            f32  s[4];
            u16  h[8];
            u8   b[16];
        } FpuRegister;

        static_assert(sizeof(CpuRegister) == sizeof(u64));
        static_assert(sizeof(FpuRegister) == sizeof(u128));

        struct InlineCtx {
            CpuRegister registers[31];
        };

        struct ExInlineCtx {
            CpuRegister registers[31];
            CpuRegister sp;
            FpuRegister v[32];
        };

        static_assert(sizeof(InlineCtx) == 0xF8);
        static_assert(sizeof(ExInlineCtx) == 0x300);

        using InlineCallback = void (*)(InlineCtx*);
        using ExInlineCallback = void (*)(ExInlineCtx*);

        static void Initialize();

        template<typename Func> requires RealFunction<Func> || std::is_member_function_pointer_v<Func>
        static Func HookFunc(Func hook, Func callback, bool do_trampoline = false) {

            /* Workaround for being unable to cast member functions. */
            /* Probably some horrible UB here? */
            uintptr_t hookp;
            uintptr_t callbackp;
            memcpy(&hookp, &hook, sizeof(hookp));
            memcpy(&callbackp, &callback, sizeof(callbackp));

            uintptr_t trampoline = HookFuncCommon(hookp, callbackp, do_trampoline);

            /* Workaround for being unable to cast member functions. */
            /* Probably some horrible UB here? */
            Func ret;
            memcpy(&ret, &trampoline, sizeof(trampoline));

            return ret;
        }

        template<typename Func> requires RealFunction<Func>
        static Func HookFunc(uintptr_t hook, Func callback, bool do_trampoline = false) {
            return HookFunc(reinterpret_cast<Func>(hook), callback, do_trampoline);
        }
        
        template<typename Func> requires RealFunction<Func>
        static Func HookFunc(uintptr_t hook, uintptr_t callback, bool do_trampoline = false) {
            return HookFunc(reinterpret_cast<Func>(hook), reinterpret_cast<Func>(callback), do_trampoline);
        }

        template<typename Func1, typename Func2> 
        requires 
        /* Both funcs are member pointers. */
        std::is_member_function_pointer_v<Func1> && std::is_member_function_pointer_v<Func2>
        /* TODO: ensure safety that Func2 can be casted to Func1 */
        static Func1 HookFunc(Func1 hook, Func2 callback, bool do_trampoline = false) {
            return HookFunc(reinterpret_cast<Func1>(hook), reinterpret_cast<Func1>(callback), do_trampoline);
        }
        
        static void InlineHook(uintptr_t hook, InlineCallback callback) {
            InlineHook(hook, reinterpret_cast<uintptr_t>(callback), false);
        }

        static void InlineHook(uintptr_t hook, ExInlineCallback callback) {
            InlineHook(hook, reinterpret_cast<uintptr_t>(callback), true);
        }
    };
};