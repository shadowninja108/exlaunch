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

#include <type_traits>

namespace exl::util {

    template<typename T>
    concept RealFunction = std::is_function_v<T> || std::is_function_v<std::remove_pointer_t<T>>;

    class Hook {
        private:
        static Jit s_HookJit;

        static bool HookFuncImpl(void* const symbol, void* const replace, void* const rxtr, void* const rwtr);
        static Result AllocForTrampoline(uint32_t** rx, uint32_t** rw);

        public:

        typedef union {
            u64 x;  ///< 64-bit AArch64 register view.
            u32 w;  ///< 32-bit AArch64 register view.
            u32 r;  ///< AArch32 register view.
        } CpuRegister;

        struct InlineCtx {
            CpuRegister registers[29];
        };
        using InlineCallback = void (*)(InlineCtx*);

        static void Initialize();

        template<typename Func> requires RealFunction<Func>
        static Func HookFunc(Func hook, Func callback, bool do_trampoline = false) {

            /* TODO: thread safety */

            R_ABORT_UNLESS(jitTransitionToWritable(&s_HookJit));

            u32* rxtrampoline = NULL;
            u32* rwtrampoline = NULL;
            if (do_trampoline) 
                R_ABORT_UNLESS(AllocForTrampoline(&rxtrampoline, &rwtrampoline));

            if (!HookFuncImpl(reinterpret_cast<void*>(hook), reinterpret_cast<void*>(callback), rxtrampoline, rwtrampoline))
                EXL_ABORT(exl::result::HookFailed);

            R_ABORT_UNLESS(jitTransitionToExecutable(&s_HookJit));


            return (Func)rxtrampoline;
        }

        template<typename Func> requires RealFunction<Func>
        static Func HookFunc(uintptr_t hook, Func callback, bool do_trampoline = false) {
            return HookFunc(reinterpret_cast<Func>(hook), callback, do_trampoline);
        }
        
        template<typename Func> requires RealFunction<Func>
        static Func HookFunc(uintptr_t hook, uintptr_t callback, bool do_trampoline = false) {
            return HookFunc(reinterpret_cast<Func>(hook), reinterpret_cast<Func>(callback), do_trampoline);
        }

    };
    //static void InlineHook(uintptr_t addr, InlineCallback* callback);
};