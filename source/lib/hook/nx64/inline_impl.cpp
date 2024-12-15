
#include <common.hpp>

#include <array>

#include "../../util/sys/jit.hpp"
#include "../../armv8.hpp"
#include "impl.hpp"

namespace exl::hook::nx64 {

    /* Size of stack to reserve for the contexts. Adjust this along with variables in inline_asm.s */
    static constexpr int CtxStackBaseSize = 0x100;
    static constexpr int CtxStackFloatSize = 0x200;

    namespace reg = exl::armv8::reg;
    namespace inst = exl::armv8::inst;

    struct Entry {
        std::array<inst::Instruction, 4> m_CbEntry;
        uintptr_t m_Callback;
    };

    static constexpr size_t InlinePoolCount = setting::InlinePoolSize / sizeof(Entry);

    JIT_CREATE(s_InlineHookJit, setting::InlinePoolSize);
    static size_t s_EntryIndex = 0;

    extern "C" {
        extern char exl_inline_hook_impl;
        extern char exl_inline_float_hook_impl;
    }

    static uintptr_t GetImpl(bool use_float_impl) {
        if(!use_float_impl) {
            return reinterpret_cast<uintptr_t>(&exl_inline_hook_impl);
        } else {
            return  reinterpret_cast<uintptr_t>(&exl_inline_float_hook_impl);
        }
    }

    static const Entry* GetEntryRx() {
        return reinterpret_cast<const Entry*>(s_InlineHookJit.GetRo());
    }

    static Entry* GetEntryRw() {
        return reinterpret_cast<Entry*>(s_InlineHookJit.GetRw());
    }

    void InitializeInline() {
        s_InlineHookJit.Initialize();
    }

    void HookInline(uintptr_t hook, uintptr_t callback, bool capture_floats) {
        /* Ensure enough space in the pool. */
        if(s_EntryIndex >= InlinePoolCount)
            R_ABORT_UNLESS(result::HookTrampolineAllocFail);

        /* Grab entry from pool. */
        auto entryRx = &GetEntryRx()[s_EntryIndex];
        auto entryRw = &GetEntryRw()[s_EntryIndex];
        s_EntryIndex++;

        /* Get pointer to entry's entrypoint. */
        uintptr_t entryCb = reinterpret_cast<uintptr_t>(&entryRx->m_CbEntry);
        /* Hook to call into the entry's entrypoint. Assign trampoline to be used by impl. */
        auto trampoline = Hook(hook, entryCb, true);
        /* Offset of LR before SP is moved. */
        static constexpr int lrBackupOffset = int(offsetof(InlineCtx, m_Gpr.m_Lr)) - CtxStackBaseSize;
        static_assert(lrBackupOffset == -0x10, "InlineCtx is not ABI compatible.");

        /* Ensure the context's sizes line up.  */
        static_assert(sizeof(InlineFloatCtx) == CtxStackBaseSize + CtxStackFloatSize, "Float context will not fit in the stack!");
        static_assert(sizeof(FloatRegisters) == CtxStackFloatSize, "Float context will not fit in stack!");

        /* Select appropriate implementation for entrypoint. */
        auto impl = GetImpl(capture_floats);

        /* Construct entrypoint instructions. */
        entryRw->m_CbEntry = {
            /* Backup LR register to stack, as we are about to trash it. */
            inst::SturUnscaledImmediate(reg::LR, reg::SP, lrBackupOffset),
            /* Branch to implementation. */
            inst::BranchLink(impl - (entryCb + (1 * sizeof(armv8::InstType)))),
            /* Restore proper LR. */
            inst::LdurUnscaledImmediate(reg::LR, reg::SP, lrBackupOffset),
            /* Branch to trampoline. */
            inst::Branch(trampoline - (entryCb + (3 * sizeof(armv8::InstType))))
        };
        /* Assign callback to be called to be used by impl. */
        entryRw->m_Callback = callback;

        /* Finally, flush caches to have RX region to be consistent. */
        s_InlineHookJit.Flush();
    }
}