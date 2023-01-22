#include "mem_layout.hpp"

#include <reloc/rtld.hpp>
#include <program/setting.hpp>
    
/* Provided by linkerscript, the start of our executable. */
extern "C" {
    extern char __module_start;
}

namespace exl::util {

    static void FindModules() {
        /* Setup loop, starting address zero. */
        MemoryInfo meminfo { };
        u32 pageinfo;

        enum class State {
            LookingForCodeStatic,
            ExpectingRodata,
            ExpectingData,
        } state = State::LookingForCodeStatic;
        int nextModIdx = 0;

        uintptr_t offset = 0;
        uintptr_t prevOffset = 0;
        util::ModuleInfo curModInfo {};

        /* Utility to reset state.  */
        auto reset = [&curModInfo, &state] {
            curModInfo = {};
            state =  State::LookingForCodeStatic;
        };

        do {
            /* Ensure we don't find too many modules. */
            if(util::mem_layout::s_MaxModules <= nextModIdx)
                EXL_ABORT(result::TooManyStaticModules);

            prevOffset = offset;

            /* Query next range. */
            R_ABORT_UNLESS(svcQueryMemory(&meminfo, &pageinfo, meminfo.addr + meminfo.size));

            /* Setup variables for state machine. */
            u32 memtype = meminfo.type & MemState_Type;
            offset = meminfo.addr;

            switch (state) {
                case State::LookingForCodeStatic: {
                    if(memtype != MemType_CodeStatic || meminfo.perm != Perm_Rx) {
                        /* No module here, keep going... */
                        continue;
                    }

                    /* Store text/total info and move to next state. */
                    curModInfo.m_Total.m_Start = meminfo.addr;
                    curModInfo.m_Text.m_Start = meminfo.addr;
                    curModInfo.m_Text.m_Size = meminfo.size;
                    state =  State::ExpectingRodata;
                    break;
                }
                case State::ExpectingRodata: {
                    if(memtype != MemType_CodeStatic || meminfo.perm != Perm_R) {
                        /* Not a proper module, reset. */
                        reset();
                        continue;
                    }

                    /* Store rodata range and move to next state. */
                    curModInfo.m_Rodata.m_Start = meminfo.addr;
                    curModInfo.m_Rodata.m_Size = meminfo.size;
                    state =  State::ExpectingData;
                    break;
                }
                case State::ExpectingData: {
                    if(memtype != MemType_CodeMutable || meminfo.perm != Perm_Rw) {
                        /* Not a proper module, reset. */
                        reset();
                        continue;
                    }

                    auto& total = curModInfo.m_Total;
                    auto& data = curModInfo.m_Data;

                    /* Assign the data range and finish total range. */
                    data.m_Start = meminfo.addr;
                    data.m_Size = meminfo.size;
                    total.m_Size = data.GetEnd() - total.m_Start;

                    /* This only needs to be determined at runtime if we are in an application. */
                    #ifdef EXL_AS_MODULE
                    if(total.m_Start == (uintptr_t) &__module_start)
                        util::mem_layout::s_SelfModuleIdx = nextModIdx;
                    #endif

                    /* Store built module info. */
                    impl::mem_layout::s_ModuleInfos[nextModIdx] = curModInfo;
                    nextModIdx++;
                    util::mem_layout::s_ModuleCount = nextModIdx;

                    /* Back to initial state. */
                    reset();
                    break;
                }
                EXL_UNREACHABLE_DEFAULT_CASE();
            }

        /* Exit once we've wrapped the address space. */
        } while(offset >= prevOffset);

        /* Ensure we found a valid self index and module count. */
        #ifdef EXL_AS_MODULE
        EXL_ASSERT(util::mem_layout::s_SelfModuleIdx != -1);
        EXL_ASSERT(util::mem_layout::s_SelfModuleIdx < util::mem_layout::s_MaxModules);
        #endif
        EXL_ASSERT(util::mem_layout::s_ModuleCount != -1);
        EXL_ASSERT(util::mem_layout::s_ModuleCount <= util::mem_layout::s_MaxModules);
    }

    static Result TryGetAddressFromInfo(InfoType type, uintptr_t* ptr) {
        return svcGetInfo(static_cast<u64*>(ptr), type, CUR_PROCESS_HANDLE, 0);
    }

    static uintptr_t GetAddressFromInfo(InfoType type) {
        uintptr_t addr;
        R_ABORT_UNLESS(TryGetAddressFromInfo(type, &addr));
        return addr;
    }

    static Result InferAslrAndStack() {
        Result rc = svcUnmapMemory((void*)0xFFFFFFFFFFFFE000UL, (void*)0xFFFFFE000UL, 0x1000);
        if (R_VALUE(rc) == KERNELRESULT(InvalidMemoryState)) {
            // Invalid src-address error means that a valid 36-bit address was rejected.
            // Thus we are 32-bit.
            util::mem_layout::s_Aslr    = util::Range(0x200000ull, 0x100000000ull);
            util::mem_layout::s_Stack   = util::Range(0x200000ull, 0x40000000ull);
        }
        else if (R_VALUE(rc) == KERNELRESULT(InvalidMemoryRange)) {
            // Invalid dst-address error means our 36-bit src-address was valid.
            // Thus we are 36-bit.
            util::mem_layout::s_Aslr    = util::Range(0x8000000ull, 0x1000000000ull);
            util::mem_layout::s_Stack   = util::Range(0x8000000ull, 0x80000000ull);
        }
        else {
            // Wat.
            return MAKERESULT(Module_Libnx, LibnxError_WeirdKernel);
        }

        return result::Success;
    }

    static void FindRegions() {
        util::mem_layout::s_Alias = util::Range(
            GetAddressFromInfo(InfoType_AliasRegionAddress),
            GetAddressFromInfo(InfoType_AliasRegionSize)
        );

        util::mem_layout::s_Heap = util::Range(
            GetAddressFromInfo(InfoType_HeapRegionAddress),
            GetAddressFromInfo(InfoType_HeapRegionSize)
        );

        if(R_FAILED(TryGetAddressFromInfo(InfoType_AslrRegionAddress,   &util::mem_layout::s_Aslr.m_Start)) ||
           R_FAILED(TryGetAddressFromInfo(InfoType_AslrRegionSize,      &util::mem_layout::s_Aslr.m_Size)) ||
           R_FAILED(TryGetAddressFromInfo(InfoType_StackRegionAddress,  &util::mem_layout::s_Stack.m_Start)) ||
           R_FAILED(TryGetAddressFromInfo(InfoType_StackRegionSize,     &util::mem_layout::s_Stack.m_Size))
        )
        {
            EXL_ASSERT(R_SUCCEEDED(InferAslrAndStack()));
        }
    }

    void impl::InitMemLayout() {
        FindModules();
        FindRegions();
    }
};