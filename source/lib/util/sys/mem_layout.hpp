#pragma once

#include "common.hpp"
#include <array>

namespace exl::util {

    struct Range {
        uintptr_t m_Start;
        size_t m_Size;

        constexpr uintptr_t GetEnd() const { return m_Start + m_Size; }
    };

    struct ModuleInfo {
        Range m_Total;
        Range m_Text;
        Range m_Rodata;
        Range m_Data;
        /* TODO: bss? */
    };

    namespace mem_layout {
        static constexpr int s_MaxModules = 13;
        inline int s_ModuleCount = -1;
        
        inline Range s_Alias;
        inline Range s_Heap;
        inline Range s_Aslr;
        inline Range s_Stack;
    }

    namespace impl {
        void InitMemLayout();

        namespace mem_layout {
            inline std::array<ModuleInfo, util::mem_layout::s_MaxModules> s_ModuleInfos;
        }
    }

    static inline const ModuleInfo& GetModuleInfo(int index) {
        EXL_ASSERT(index < mem_layout::s_ModuleCount); 
        return impl::mem_layout::s_ModuleInfos.at(index); 
    }

    namespace mem_layout {

    #if defined EXL_AS_KIP
        /* TODO */
        #error "Not implemented..."
    #elif defined EXL_AS_RTLD
        static constexpr int s_RtldModuleIdx = 0;
        static constexpr int s_MainModuleIdx = 1;
        static constexpr int s_SelfModuleIdx = s_RtldModuleIdx;

        }

    static inline const ModuleInfo& GetRtldModuleInfo() { return GetModuleInfo(mem_layout::s_RtldModuleIdx); }
    static inline const ModuleInfo& GetMainModuleInfo() { return GetModuleInfo(mem_layout::s_MainModuleIdx); }
    #elif defined EXL_AS_MODULE
        static constexpr int s_RtldModuleIdx = 0;
        static constexpr int s_MainModuleIdx = 1;

        /* Decided at runtime. */
        inline int s_SelfModuleIdx = -1;

        }
    static inline const ModuleInfo& GetRtldModuleInfo() { return GetModuleInfo(mem_layout::s_RtldModuleIdx); }
    static inline const ModuleInfo& GetMainModuleInfo() { return GetModuleInfo(mem_layout::s_MainModuleIdx); }
    #endif

    
    static inline const ModuleInfo& GetSelfModuleInfo() { return GetModuleInfo(mem_layout::s_SelfModuleIdx); }
    static inline const ModuleInfo& GetSdkModuleInfo() { return GetModuleInfo(mem_layout::s_ModuleCount - 1); }
};