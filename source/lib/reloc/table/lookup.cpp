#include "lookup.hpp"

#include <rtld.hpp>
#include <bitset>
#include <cstring>
#include <lib/util/module_index.hpp>
#include <lib/util/sys/mem_layout.hpp>
#include <lib/util/strings.hpp>
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
            return std::string_view(ptr, strlen(ptr));
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
        template<typename T>
        requires IsElfRel<T>
        void ApplyRel(const LookupEntryBin& entry, RelAccessor<T> accessor, std::string_view symbolName) {
            auto type = accessor.GetType();
            if(
                /* If it's not a jump slot, rel absolute, or a global data, we will not apply it. */
                type != ARCH_JUMP_SLOT &&
                !(ARCH_IS_REL_ABSOLUTE(type)) &&
                type != ARCH_GLOB_DAT
            ) {
                return;
            }

            if(!util::HasModule(entry.m_ModuleIndex)) {
                Logging.Log(EXL_LOG_PREFIX "Symbol %s has invalid module index %d", symbolName.data(), static_cast<int>(entry.m_ModuleIndex));
                return;
            }

            auto target = accessor.GetTarget();
            auto referredModule = util::GetModuleInfo(entry.m_ModuleIndex);
            if(Logging.IsEnabled()) {
                char nameBuffer[util::ModuleInfo::s_ModulePathLengthMax+1];
                util::CopyString(nameBuffer, referredModule.GetModuleName());
                Logging.Log(EXL_LOG_PREFIX "Symbol %s being relocated to [%s]+%x", symbolName.data(), nameBuffer, entry.m_Offset);
            }

            /* Target address will be (referred module start + offset) + rel addend. */
            *target = static_cast<uintptr_t>(referredModule.m_Total.m_Start + entry.m_Offset + accessor.GetAddend());
        }

        template<typename T>
        requires IsElfRel<T>
        void ApplyRels(const Lookup& table, const rtld::ModuleObject& mod, const T* ptr, size_t count, size_t size) {
            std::span<const T> span { ptr, std::max(count, size / sizeof(T)) };
            for(auto rel : span) {
                auto accessor = RelAccessor<T> { rel, mod };
                auto sym = accessor.GetSym();
                auto search = table.FindByName(sym.GetName());
                if(search != nullptr) {
                    ApplyRel(*search, accessor, sym.GetName());
                }
            }
        }

        void ApplyRels(const Lookup& table, const rtld::ModuleObject& mod) {
            bool hasRel = (mod.rel_count != 0) || (mod.rel_dyn_size != 0);
            bool hasRela = (mod.rela_count != 0) || (mod.rela_dyn_size != 0);

            EXL_ASSERT(hasRel != hasRela);
            /* Seems dkp's linker omits this tag, so RTLD ends up misunderstanding. Fortunately, this seems to work out (?). */
            // EXL_ASSERT(hasRela == mod.is_rela);
            
            if(hasRela) {
                ApplyRels<Elf_Rela>(table, mod, mod.rela_or_rel.rela, mod.rela_count, mod.rela_dyn_size);
            } else {
                ApplyRels<Elf_Rel>(table, mod, mod.rela_or_rel.rel, mod.rel_count, mod.rel_dyn_size);
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
    }
}