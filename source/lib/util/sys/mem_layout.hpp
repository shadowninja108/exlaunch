#pragma once

#include <common.hpp>
#include <lib/util/module_index.hpp>
#include <array>
#include <bitset>
#include <cstdint>
#include <string_view>
#include <rtld.hpp>

namespace exl::util {

    struct Range {
        uintptr_t m_Start;
        size_t m_Size;

        constexpr uintptr_t GetEnd() const { return m_Start + m_Size; }

        constexpr bool InRange(uintptr_t value) const {
            return m_Start <= value && value < GetEnd();
        }

        constexpr bool InRangeInclusive(uintptr_t value) const {
            return m_Start <= value && value <= GetEnd();
        }
    };

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
        const Elf_Dyn* GetDynamic()     const { return reinterpret_cast<const Elf_Dyn*>(GetPointer(m_DynamicStartOffset)); }
        uintptr_t GetBssStart()         const { return GetPointer(m_BssStartOffset); }
        uintptr_t GetBssEnd()           const { return GetPointer(m_BssEndOffset); }
        uintptr_t GetEhFrameHdrStart()  const { return GetPointer(m_EhFrameHdrStartOffset); }
        uintptr_t GetEhFrameHdrEnd()    const { return GetPointer(m_EhFrameHdrEndOffset); }

        Range GetBss()                  const { return { GetBssStart(), GetBssEnd() - GetBssStart() }; }
        Range GetEhFrameHdr()           const { return { GetEhFrameHdrStart(), GetEhFrameHdrEnd() - GetEhFrameHdrStart() }; }
    };

    struct ModuleInfo {
        static constexpr size_t s_ModulePathLengthMax = 0x200;

        Range m_Total;
        Range m_Text;
        Range m_Rodata;
        Range m_Data;
        const Mod0* m_Mod;

        std::string_view GetModulePath() const {
            struct {
                std::uint32_t m_Unk;
                std::uint32_t m_Length;
                char m_Data[s_ModulePathLengthMax];
            }* module;
            auto ptr = reinterpret_cast<decltype(module)>(m_Rodata.m_Start);
            if(ptr->m_Length == 0)
                return "";

            return { ptr->m_Data, ptr->m_Length };
        }

        std::string_view GetModuleName() const {
            auto path = GetModulePath();
            if(path.empty())
                return path;

            /* Try to find the string after the last slash. */
            auto pos = path.find_last_of("/\\");
            if(pos == std::string_view::npos)
                pos = 0;
            else
                pos++;

            /* If it goes out of bounds somehow, just return the path. */
            if(path.size() < pos)
                return path;

            return path.substr(pos);
        }
    };

    namespace mem_layout {
        inline Range s_Alias;
        inline Range s_Heap;
        inline Range s_Aslr;
        inline Range s_Stack;
    }

    namespace impl {
        void InitMemLayout();

        namespace mem_layout {
            extern std::array<ModuleInfo, static_cast<int>(ModuleIndex::End)> s_ModuleInfos;
            extern std::bitset<static_cast<int>(ModuleIndex::End)> s_ModuleBitset;
        }
    }

    [[gnu::const]] inline bool HasModule(ModuleIndex index) {
        return impl::mem_layout::s_ModuleBitset[static_cast<int>(index)];
    }

    [[gnu::const]] inline const ModuleInfo& GetModuleInfo(ModuleIndex index) {
        EXL_ABORT_UNLESS(HasModule(index));
        return impl::mem_layout::s_ModuleInfos[static_cast<int>(index)];
    }

    namespace mem_layout {

    #ifdef EXL_AS_KIP
        /* TODO */
        #error "Not implemented..."
    }
    #elif defined EXL_AS_MODULE
        /* Decided at runtime. */
        extern ModuleIndex s_SelfModuleIdx;
    }
    #elif defined EXL_AS_RTLD
        constexpr auto s_SelfModuleIdx = ModuleIndex::Rtld;
    #endif


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