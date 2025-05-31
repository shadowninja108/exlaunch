#pragma once

#include <common.hpp>
#include <rtld.hpp>

#include "range.hpp"

namespace exl::util {
    
    struct Mod0 {
        NON_COPYABLE(Mod0);
        NON_MOVEABLE(Mod0);

        char m_Magic[4];
        std::int32_t m_DynamicStartOffset;
        std::int32_t m_BssStartOffset;
        std::int32_t m_BssEndOffset;
        std::int32_t m_EhFrameHdrStartOffset;
        std::int32_t m_EhFrameHdrEndOffset;
        std::int32_t m_ModuleRuntimeOffset;

        private:
        uintptr_t GetPointer(std::int32_t offset) const {
            return reinterpret_cast<uintptr_t>(this) + offset;
        }

        public:
        const Elf_Dyn* GetDynamic()                     const { return reinterpret_cast<const Elf_Dyn*>(GetPointer(m_DynamicStartOffset)); }
        uintptr_t GetBssStart()                         const { return GetPointer(m_BssStartOffset); }
        uintptr_t GetBssEnd()                           const { return GetPointer(m_BssEndOffset); }
        uintptr_t GetEhFrameHdrStart()                  const { return GetPointer(m_EhFrameHdrStartOffset); }
        uintptr_t GetEhFrameHdrEnd()                    const { return GetPointer(m_EhFrameHdrEndOffset); }
        const rtld::ModuleObject* GetModuleRuntime()    const { return reinterpret_cast<rtld::ModuleObject*>(GetPointer(m_ModuleRuntimeOffset)); }

        Range GetBss()                                  const { return { GetBssStart(), GetBssEnd() - GetBssStart() }; }
        Range GetEhFrameHdr()                           const { return { GetEhFrameHdrStart(), GetEhFrameHdrEnd() - GetEhFrameHdrStart() }; }

    };

}