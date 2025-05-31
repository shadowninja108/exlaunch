#pragma once

#include <cstdint>
#include <algorithm>
#include <bit>
#include <functional>
#include <ranges>
#include <tuple>
#include <cstring>
#include <rtld.hpp>
#include "../util/sys/mem_layout.hpp"
#include "table/lookup_entry.hpp"
#include "table/lookup.hpp"
#include "table/table.hpp"
#include "table/table_set.hpp"

namespace exl::reloc {

    namespace impl {
        using ElfHashType = unsigned long;
        
        void Initialize();
        extern Lookup s_CachedLookup;

        inline const rtld::ModuleObject* GetModuleRuntime(util::ModuleIndex index) {
            EXL_ABORT_UNLESS(util::HasModule(index));
            
            const auto& info = util::GetModuleInfo(index);
            auto runtime = const_cast<rtld::ModuleObject*>(info.m_Mod->GetModuleRuntime());
            
            /* If the base is already initialized, assume it's been initialized already. */
            if(EXL_LIKELY(runtime->module_base != nullptr))
                return runtime;

            /* Initialize the module runtime. */
            runtime->Initialize(
                reinterpret_cast<char*>(info.m_Total.m_Start),
                const_cast<Elf_Dyn*>(info.m_Mod->GetDynamic())
            );

            /* Then return it. */
            return runtime;
        }

        /* Duplicated to allow for constexpr. */
        inline constexpr ElfHashType ElfHash(const char *name) {
            ElfHashType h = 0;
            ElfHashType g;

            while (*name) {
                h = (h << 4) + *name++;
                if ((g = h & 0xf0000000)) h ^= g >> 24;
                h &= ~g;
            }
            return h;
        }
    }


    [[gnu::const]] 
    inline const Lookup& GetLookupTable() { return impl::s_CachedLookup; }

    inline Elf_Sym* GetSymbol(util::ModuleIndex index, const char* name) {
        auto module = impl::GetModuleRuntime(index);
        impl::ElfHashType hash = impl::ElfHash(name);

        for (uint32_t i = module->hash_bucket[hash % module->hash_nbucket_value];
            i; i = module->hash_chain[i]) {
            bool is_common = module->dynsym[i].st_shndx
                                ? module->dynsym[i].st_shndx == SHN_COMMON
                                : true;
            if (!is_common &&
                std::strcmp(name, module->dynstr + module->dynsym[i].st_name) == 0) {
                return &module->dynsym[i];
            }
        }

        return nullptr;
    }

    inline Elf_Sym* GetSymbol(const char* name) {
        for(auto i = static_cast<int>(util::ModuleIndex::Start); i < static_cast<int>(util::ModuleIndex::End); i++) {
            auto index = static_cast<util::ModuleIndex>(i);
            /* Ignore non-existent modules. */
            if(!util::HasModule(index))
                continue;
             
            auto res = GetSymbol(index, name);
            if(res != nullptr)
                return res;
        }

    }
}
