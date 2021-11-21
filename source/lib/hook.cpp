#include "hook.hpp"

#include "mem_layout.hpp"
#include "util/lazy_init.hpp"

namespace exl::hook {

    namespace {
        LazyInit<MemLayout> s_MemLayout;
    }
    
    using entrypoint_t = void (*)(void*,void*);

    void NORETURN CallTargetEntrypoint(void* x0, void* x1) {
        auto entrypoint = (entrypoint_t) GetTargetStart();
        entrypoint(x0, x1);
        UNREACHABLE;
    }

    uintptr_t GetSelfStart() {
        return s_MemLayout->m_SelfStart;
    }

    uintptr_t GetTargetStart() {
        return GetTargetOffset(0);
    }
    
    uintptr_t GetTargetOffset(uintptr_t offset) {
        return s_MemLayout->m_TargetStart + offset;
    }

    void Initialize() {
        util::Hook::Initialize();
    } 

};