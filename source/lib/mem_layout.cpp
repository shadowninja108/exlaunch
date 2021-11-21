#include "mem_layout.hpp"

#include "reloc/rtld.hpp"
#include "program/setting.hpp"
    
/* Provided by linkerscript, the start of our executable. */
extern "C" {
    extern char __module_start;
}

namespace exl {
    
    /* Find the target for when we are RTLD. */
    uintptr_t MemLayout::FindTargetAsRtld() {
        MemoryInfo meminfo;
        u32 pageinfo;

        /* Iterate memory forwards from self to find target. */
        u64 addr = m_SelfStart;
        do {
            R_ABORT_UNLESS(svcQueryMemory(&meminfo, &pageinfo, addr));

            if( meminfo.addr != m_SelfStart &&      /* Ignore ourselves. */
                meminfo.perm == Perm_Rx     &&      /* Looking for executables. */
                meminfo.type == MemType_CodeStatic) /* Looking for *static* executables. */
                break;
            
            /* Move to next area. */
            addr += meminfo.size;
        } while(true);

        return meminfo.addr;
    }

    /* Dynamic link to nnMain, used to find target. */
    using entrypoint_t = void(*)();
    extern "C" {
        extern entrypoint_t* nnMain;
    };

    /* Use RTLD's info to find the target. */
    uintptr_t MemLayout::FindTargetFromRtld() {
        /* Get address for nnMain. */
        auto mainAddr = (uintptr_t) &nnMain;

        /* Ensure that nnMain address exists. */
        if(mainAddr == 0)
            EXL_ABORT(result::FailedToFindTarget);

        /* Find .text segment that nnMain is contained, which will be the start of target. */
        MemoryInfo meminfo;
        u32 pageinfo;
        R_ABORT_UNLESS(svcQueryMemory(&meminfo, &pageinfo, mainAddr));

        return meminfo.addr;
    }

    void MemLayout::Initialize() {
        MemoryInfo meminfo;
        u32 pageinfo;

        /* Get location of self. */
        R_ABORT_UNLESS(svcQueryMemory(&meminfo, &pageinfo, (u64)&__module_start));
        m_SelfStart = meminfo.addr;

        /* Finding the target depends on how we are loaded. */
        switch(setting::SelfLoadKind) {
            case setting::LoadKind::AsRtld:
                m_TargetStart = FindTargetAsRtld();
                break;
            case setting::LoadKind::Module:
                m_TargetStart = FindTargetFromRtld();
                break;
            
            /* TODO: KIPs? */
            EXL_UNREACHABLE_DEFAULT_CASE();
        }
    }
};