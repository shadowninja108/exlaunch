#include "lookup.hpp"

#include <rtld.hpp>
#include <bitset>
#include <cstring>
#include <lib/util/module_index.hpp>
#include <lib/util/sys/mem_layout.hpp>
#include <lib/util/strings.hpp>
#include <lib/util/sys/modules.hpp>
#include <lib/reloc/reloc.hpp>
#include <lib/log/logger_mgr.hpp>
#include <program/loggers.hpp>

#include <lib/util/sys/mem_layout.hpp>

namespace exl::reloc {
    
    template<typename T>
    concept IsElfRel = std::same_as<T, Elf_Rel> || std::same_as<T, Elf_Rela>;

    struct SymAccessor {
        const Elf_Sym& m_Ref;
        const rtld::ModuleObject& m_Mod;

        std::string_view GetName() {
            auto ptr = &m_Mod.dynstr[m_Ref.st_name];
            return std::string_view(ptr, std::strlen(ptr));
        }
    };

    template<typename T>
    requires IsElfRel<std::remove_const_t<T>>
    struct RelAccessorBase {
        const T& m_Ref;
        const rtld::ModuleObject& m_Mod;

        uintptr_t GetOffset() const { return m_Ref.r_offset; }
        std::uint32_t GetType() const { return ELF_R_TYPE(m_Ref.r_info); }

        SymAccessor GetSym() const {
            /* No way to know the length...? */
            return { m_Mod.dynsym[ELF_R_SYM(m_Ref.r_info)], m_Mod };
        }
        auto GetTarget() const { 
            return reinterpret_cast<uintptr_t*>(m_Mod.module_base + GetOffset()); 
        }
        auto GetTargetAddress() const { 
            return reinterpret_cast<const uintptr_t*>(m_Mod.module_base + *GetTarget()); 
        }
    };

    template<typename T>
    struct RelAccessor{};

    template<>
    struct RelAccessor<const Elf_Rel> : public RelAccessorBase<const Elf_Rel> {
        ptrdiff_t GetAddend() const { return 0; }
    };
    template<>
    struct RelAccessor<const Elf_Rela> : public RelAccessorBase<const Elf_Rela>  {
        ptrdiff_t GetAddend() const { return m_Ref.r_addend; }
    };

    template<>
    struct RelAccessor<Elf_Rel> : RelAccessor<const Elf_Rel> {};
    template<>
    struct RelAccessor<Elf_Rela> : RelAccessor<const Elf_Rela> {};

    namespace {

        void ApplyRel(const LookupEntryBin& entry, std::string_view symbolName, std::uint32_t type, uintptr_t* target, ptrdiff_t addend) {
            auto referredModule = util::GetModuleInfo(entry.m_ModuleIndex);
            if(Logging.IsEnabled()) {
                char nameBuffer[util::ModuleInfo::s_ModulePathLengthMax+1];
                util::CopyString(nameBuffer, referredModule.GetModuleName());
                Logging.Log(EXL_LOG_PREFIX "Symbol %s being relocated to [%s]+%x", symbolName.data(), nameBuffer, entry.m_Offset);
            }

            /* Target address will be (referred module start + offset) + rel addend. */
            *target = static_cast<uintptr_t>(referredModule.m_Total.m_Start + entry.m_Offset + addend);
        }

        void CheckAndApplyRel(const Lookup& table, SymAccessor symbol, std::uint32_t type, uintptr_t* target, ptrdiff_t addend) {
            auto symbolName = symbol.GetName();
            auto search = table.FindByName(symbolName);
            if(search == nullptr)
                return;
                
            if(
                /* If it's not a jump slot, rel absolute, or a global data, we will not apply it. */
                type != ARCH_JUMP_SLOT &&
                !(ARCH_IS_REL_ABSOLUTE(type)) &&
                type != ARCH_GLOB_DAT
            ) {
                return;
            }


            if(!util::HasModule(search->m_ModuleIndex)) {
                Logging.Log(EXL_LOG_PREFIX "Symbol %s has invalid module index %d", symbolName.data(), static_cast<int>(search->m_ModuleIndex));
                return;
            }

            ApplyRel(*search, symbolName, type, target, addend);
        }

        template<typename T>
        requires IsElfRel<T>
        void ApplyRels(const Lookup& table, const rtld::ModuleObject& mod, const T* ptr, size_t size) {
            std::span<const T> span { ptr, size / sizeof(T) };
            for(auto rel : span) {
                auto accessor = RelAccessor<T> { rel, mod };
                CheckAndApplyRel(
                    table,
                    accessor.GetSym(),
                    accessor.GetType(),
                    accessor.GetTarget(),
                    accessor.GetAddend()
                );
            }
        }

        void ApplyRels(const Lookup& table, const rtld::ModuleObject& mod) {
            bool hasRel = (mod.rel_count != 0) || (mod.rel_dyn_size != 0);
            bool hasRela = (mod.rela_count != 0) || (mod.rela_dyn_size != 0);

            EXL_ASSERT(hasRel != hasRela);
            
            if(hasRela) {
                ApplyRels<Elf_Rela>(table, mod, mod.rela_or_rel.rela, mod.rela_dyn_size);
            } else {
                ApplyRels<Elf_Rel>(table, mod, mod.rela_or_rel.rel, mod.rel_dyn_size);
            }
        }

        void ApplyPlt(const Lookup& table, const rtld::ModuleObject& mod) {
            if(mod.is_rela) {
                ApplyRels<Elf_Rela>(table, mod, mod.rela_or_rel_plt.rela, mod.rela_or_rel_plt_size);
            } else {
                ApplyRels<Elf_Rel>(table, mod, mod.rela_or_rel_plt.rel, mod.rela_or_rel_plt_size);
            }
        }
    }

    void Lookup::Apply() const {
        Logging.Log(EXL_LOG_PREFIX "Applying relocations...");
        auto info = util::GetSelfModuleInfo();
        rtld::ModuleObject modObj = {};
        modObj.Initialize(
            reinterpret_cast<char*>(info.m_Total.m_Start), 
            const_cast<Elf_Dyn*>(info.m_Mod->GetDynamic())
        );
        ApplyRels(*this, modObj);
        ApplyPlt(*this, modObj);
    }
}