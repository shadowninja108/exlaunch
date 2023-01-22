
#include <common.hpp>

#include <array>
#include <lib.hpp>

#include "impl.hpp"

namespace exl::hook::nx64 {

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
    }

    static uintptr_t GetImpl() {
        return reinterpret_cast<uintptr_t>(&exl_inline_hook_impl);
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

    void HookInline(uintptr_t hook, uintptr_t callback) {
        /* Ensure enough space in the pool. */
        if(s_EntryIndex >= InlinePoolCount)
            EXL_ABORT(result::HookTrampolineAllocFail);

        /* Grab entry from pool. */
        auto entryRx = &GetEntryRx()[s_EntryIndex];
        auto entryRw = &GetEntryRw()[s_EntryIndex];
        s_EntryIndex++;

        /* Get pointer to entry's entrypoint. */
        uintptr_t entryCb = reinterpret_cast<uintptr_t>(&entryRx->m_CbEntry);
        /* Hook to call into the entry's entrypoint. Assign trampoline to be used by impl. */
        auto trampoline = Hook(hook, entryCb, true);

        /* Construct entrypoint instructions. */
        auto impl = GetImpl();
        entryRw->m_CbEntry = {
            /* Backup LR register to stack, as we are about to trash it. */
            inst::SturUnscaledImmediate(reg::LR, reg::SP, -0x10),
            /* Branch to implementation. */
            inst::BranchLink(impl - (entryCb + 4)),
            /* Restore proper LR. */
            inst::LdurUnscaledImmediate(reg::LR, reg::SP, -0x10),
            /* Branch to trampoline. */
            inst::Branch(trampoline - (entryCb + 8))
        };
        /* Assign callback to be called to be used by impl. */
        entryRw->m_Callback = callback;

        /* Finally, flush caches to have RX region to be consistent. */
        s_InlineHookJit.Flush();
    }
}