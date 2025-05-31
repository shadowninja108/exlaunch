#pragma once

#include "module_info.hpp"
#include "mem_layout.hpp"

namespace exl::util {
    namespace mem_layout {
    #ifdef EXL_AS_KIP
        /* TODO */
        #error "Not implemented..."
    #elif defined EXL_AS_MODULE
        /* Decided at runtime. */
        extern ModuleIndex s_SelfModuleIdx;
    #elif defined EXL_AS_RTLD
        constexpr auto s_SelfModuleIdx = ModuleIndex::Rtld;
    #endif
    }


    #ifndef EXL_AS_KIP
    static inline const ModuleInfo& GetRtldModuleInfo() { return GetModuleInfo(ModuleIndex::Rtld); }
    static inline const ModuleInfo& GetMainModuleInfo() { return GetModuleInfo(ModuleIndex::Main); }
    static inline const ModuleInfo& GetSelfModuleInfo() { return GetModuleInfo(mem_layout::s_SelfModuleIdx); }
    #endif

    inline const ModuleInfo* TryGetModule(uintptr_t pointer) {
        for(size_t i = static_cast<int>(ModuleIndex::Start); i < static_cast<int>(ModuleIndex::End); i++) {
            /* Ignore non-existent modules. */
            if(!impl::mem_layout::s_ModuleBitset[i])
                continue;
                
            const ModuleInfo* module = &impl::mem_layout::s_ModuleInfos[i];

            if(module->m_Total.InRange(pointer))
                return module;
        }

        return nullptr;
    }

    inline bool IsInModule(uintptr_t pointer, ModuleIndex module) {
        if(!HasModule(module))
            return false;
        
        return GetModuleInfo(module).m_Total.InRange(pointer);
    }
}