#include "ModuleObject.hpp"

#include <string.h>

#include "lib/reloc/rtld.hpp"
#include "lib/diag/assert.hpp"
#include "utils.hpp"

namespace rtld {

void ModuleObject::Initialize(char *aslr_base, Elf_Dyn *dynamic) {
#ifdef __RTLD_6XX__
    this->nro_size = 0;
    this->soname_idx = 0;
    this->cannot_revert_symbols = false;
#endif

    this->is_rela = false;
    this->module_base = aslr_base;
    this->dynamic = dynamic;
    this->rela_or_rel_plt_size = 0;
    this->dt_init = nullptr;
    this->dt_fini = nullptr;
    this->hash_bucket = nullptr;
    this->hash_chain = nullptr;
    this->dynstr = nullptr;
    this->dynsym = nullptr;
    this->dynstr_size = 0;
    this->rela_or_rel.rela = nullptr;
    this->got = nullptr;
    this->rela_dyn_size = 0;
    this->rel_dyn_size = 0;
    this->got_stub_ptr = 0;

    void *rel_plt = nullptr;

    for (; dynamic->d_tag != DT_NULL; dynamic++) {
        switch (dynamic->d_tag) {
            case DT_PLTRELSZ: {
                this->rela_or_rel_plt_size = dynamic->d_un.d_val;
                break;
            }

            case DT_PLTGOT: {
                this->got = (void **)(aslr_base + dynamic->d_un.d_val);
                break;
            }

            case DT_HASH: {
                uint32_t *hash_table =
                    (uint32_t *)(aslr_base + dynamic->d_un.d_val);

                const uint32_t nbucket = hash_table[0];
                const uint32_t nchain = hash_table[1];

                this->hash_nbucket_value = nbucket;
                this->hash_nchain_value = nchain;
                this->hash_bucket = &hash_table[2];
                this->hash_chain = &hash_table[2 + nbucket];
                break;
            }

            case DT_STRTAB: {
                this->dynstr = (char *)(aslr_base + dynamic->d_un.d_val);
                break;
            }

            case DT_SYMTAB: {
                this->dynsym = (Elf_Sym *)(aslr_base + dynamic->d_un.d_val);
                break;
            }

            case DT_REL: {
                this->rela_or_rel.rel =
                    (Elf_Rel *)(aslr_base + dynamic->d_un.d_val);
                break;
            }

            case DT_RELA: {
                this->rela_or_rel.rela =
                    (Elf_Rela *)(aslr_base + dynamic->d_un.d_val);
                break;
            }

            case DT_RELASZ: {
                this->rela_dyn_size = dynamic->d_un.d_val;
                break;
            }

            case DT_RELAENT: {
                EXL_ASSERT(dynamic->d_un.d_val != sizeof(Elf_Rela));
                break;
            }

            case DT_SYMENT: {
                EXL_ASSERT(dynamic->d_un.d_val == sizeof(Elf_Sym));
                break;
            }

            case DT_STRSZ: {
                this->dynstr_size = dynamic->d_un.d_val;
                break;
            }

            case DT_INIT: {
                this->dt_init = (void (*)())(aslr_base + dynamic->d_un.d_val);
                break;
            }

            case DT_FINI: {
                this->dt_fini = (void (*)())(aslr_base + dynamic->d_un.d_val);
                break;
            }

            case DT_RELSZ: {
                this->rel_dyn_size = dynamic->d_un.d_val;
                break;
            }

            case DT_RELENT: {
                EXL_ASSERT(dynamic->d_un.d_val == sizeof(Elf_Rel));
                break;
            }

            case DT_PLTREL: {
                Elf_Xword value = dynamic->d_un.d_val;
                this->is_rela = value == DT_RELA;
                EXL_ASSERT(value == DT_REL || value == DT_RELA);
                break;
            }

            case DT_JMPREL: {
                rel_plt = (void *)(aslr_base + dynamic->d_un.d_val);
                break;
            }

            case DT_RELACOUNT:
                this->rela_count = dynamic->d_un.d_val;
                break;

            case DT_RELCOUNT:
                this->rel_count = dynamic->d_un.d_val;
                break;

            case DT_SONAME:
#ifdef __RTLD_6XX__
                this->soname_idx = dynamic->d_un.d_val;
#endif
                break;

            case DT_NEEDED:
            case DT_RPATH:
            case DT_SYMBOLIC:
            case DT_DEBUG:
            case DT_TEXTREL:
            default:
                break;
        }
    }

    this->rela_or_rel_plt.raw = rel_plt;
}

void ModuleObject::Relocate() {
    if (this->rel_count) {
        for (auto i = 0u; i < this->rel_count; i++) {
            Elf_Rel *entry = &this->rela_or_rel.rel[i];
            switch (ELF_R_TYPE(entry->r_info)) {
                case ARCH_RELATIVE: {
                    Elf_Addr *ptr =
                        (Elf_Addr *)(this->module_base + entry->r_offset);
                    *ptr += (Elf_Addr)this->module_base;
                    break;
                }
            }
        }
    }

    if (this->rela_count) {
        for (auto i = 0u; i < this->rela_count; i++) {
            Elf_Rela *entry = &this->rela_or_rel.rela[i];
            switch (ELF_R_TYPE(entry->r_info)) {
                case ARCH_RELATIVE: {
                    Elf_Addr *ptr =
                        (Elf_Addr *)(this->module_base + entry->r_offset);
                    *ptr = (Elf_Addr)this->module_base + entry->r_addend;
                    break;
                }
            }
        }
    }
}

Elf_Sym *ModuleObject::GetSymbolByName(const char *name) {
    unsigned long name_hash = __rtld_elf_hash(name);

    for (uint32_t i = this->hash_bucket[name_hash % this->hash_nbucket_value];
         i; i = this->hash_chain[i]) {
        bool is_common = this->dynsym[i].st_shndx
                             ? this->dynsym[i].st_shndx == SHN_COMMON
                             : true;
        if (!is_common &&
            strcmp(name, this->dynstr + this->dynsym[i].st_name) == 0) {
            return &this->dynsym[i];
        }
    }

    return nullptr;
}

bool ModuleObject::TryResolveSymbol(Elf_Addr *target_symbol_address,
                                    Elf_Sym *symbol) {
    const char *name = &this->dynstr[symbol->st_name];

    if (ELF_ST_VISIBILITY(symbol->st_other)) {
        Elf_Sym *target_symbol = this->GetSymbolByName(name);
        if (target_symbol) {
            *target_symbol_address =
                (Elf_Addr)this->module_base + target_symbol->st_value;
            return true;
        } else if ((ELF_ST_BIND(symbol->st_info) & STB_WEAK) == STB_WEAK) {
            *target_symbol_address = 0;
            return true;
        }
    } else {
        Elf_Addr address = lookup_global_auto(name);

        if (address == 0 && ro::g_LookupGlobalManualFunctionPointer) {
            address = ro::g_LookupGlobalManualFunctionPointer(name);
        }

        if (address != 0) {
            *target_symbol_address = address;
            return true;
        }
    }
    return false;
}

void ModuleObject::ResolveSymbolRelAbsolute(Elf_Rel *entry) {
    uint32_t r_type = ELF_R_TYPE(entry->r_info);
    uint32_t r_sym = ELF_R_SYM(entry->r_info);

    if (ARCH_IS_REL_ABSOLUTE(r_type) || r_type == ARCH_GLOB_DAT) {
        Elf_Sym *symbol = &this->dynsym[r_sym];
        Elf_Addr target_symbol_address;

        if (this->TryResolveSymbol(&target_symbol_address, symbol)) {
            Elf_Addr *target =
                (Elf_Addr *)(this->module_base + entry->r_offset);
            *target += target_symbol_address;
        } else if (ro::g_RoDebugFlag) {
            print_unresolved_symbol(&this->dynstr[symbol->st_name]);
        }
    }
}

void ModuleObject::ResolveSymbolRelaAbsolute(Elf_Rela *entry) {
    uint32_t r_type = ELF_R_TYPE(entry->r_info);
    uint32_t r_sym = ELF_R_SYM(entry->r_info);

    if (ARCH_IS_REL_ABSOLUTE(r_type) || r_type == ARCH_GLOB_DAT) {
        Elf_Sym *symbol = &this->dynsym[r_sym];
        Elf_Addr target_symbol_address;

        if (this->TryResolveSymbol(&target_symbol_address, symbol)) {
            Elf_Addr *target =
                (Elf_Addr *)(this->module_base + entry->r_offset);
            *target = target_symbol_address + entry->r_addend;
        } else if (ro::g_RoDebugFlag) {
            print_unresolved_symbol(&this->dynstr[symbol->st_name]);
        }
    }
}

void ModuleObject::ResolveSymbolRelJumpSlot(Elf_Rel *entry,
                                            bool do_lazy_got_init) {
    uint32_t r_type = ELF_R_TYPE(entry->r_info);
    uint32_t r_sym = ELF_R_SYM(entry->r_info);

    if (r_type == ARCH_JUMP_SLOT) {
        Elf_Addr *target = (Elf_Addr *)(this->module_base + entry->r_offset);
        Elf_Addr target_address = (Elf_Addr)this->module_base + *target;
        if (do_lazy_got_init) {
            *target = target_address;
        }

        if (this->got_stub_ptr) {
            EXL_ASSERT(this->got_stub_ptr == (void*) target_address);
        } else {
            this->got_stub_ptr = (void *)target_address;
        }

        // We are in the non lazy case
        if (!do_lazy_got_init) {
            Elf_Sym *symbol = &this->dynsym[r_sym];
            Elf_Addr target_symbol_address;

            if (this->TryResolveSymbol(&target_symbol_address, symbol)) {
                *target += target_symbol_address;
            } else {
                if (ro::g_RoDebugFlag) {
                    print_unresolved_symbol(&this->dynstr[symbol->st_name]);
                }
                *target = target_address;
            }
        }
    }
}

void ModuleObject::ResolveSymbolRelaJumpSlot(Elf_Rela *entry,
                                             bool do_lazy_got_init) {
    uint32_t r_type = ELF_R_TYPE(entry->r_info);
    uint32_t r_sym = ELF_R_SYM(entry->r_info);

    if (r_type == ARCH_JUMP_SLOT) {
        Elf_Addr *target = (Elf_Addr *)(this->module_base + entry->r_offset);
        Elf_Addr target_address = (Elf_Addr)this->module_base + *target;
        if (do_lazy_got_init) {
            *target = target_address;
        }

        if (this->got_stub_ptr) {
            EXL_ASSERT(this->got_stub_ptr == (void*) target_address);
        } else {
            this->got_stub_ptr = (void *)target_address;
        }

        // We are in the non lazy case
        if (!do_lazy_got_init) {
            Elf_Sym *symbol = &this->dynsym[r_sym];
            Elf_Addr target_symbol_address;

            if (this->TryResolveSymbol(&target_symbol_address, symbol)) {
                *target = target_symbol_address + entry->r_addend;
            } else {
                if (ro::g_RoDebugFlag) {
                    print_unresolved_symbol(&this->dynstr[symbol->st_name]);
                }
                *target = target_address;
            }
        }
    }
}

void ModuleObject::ResolveSymbols(bool do_lazy_got_init) {
    for (auto index = this->rel_count;
         index < this->rel_dyn_size / sizeof(Elf_Rel); index++) {
        Elf_Rel *entry = &this->rela_or_rel.rel[index];
        this->ResolveSymbolRelAbsolute(entry);
    }

    for (auto index = this->rela_count;
         index < this->rela_dyn_size / sizeof(Elf_Rela); index++) {
        Elf_Rela *entry = &this->rela_or_rel.rela[index];
        this->ResolveSymbolRelaAbsolute(entry);
    }

    if (this->is_rela) {
        if (this->rela_or_rel_plt_size >= sizeof(Elf_Rela)) {
            for (auto index = 0u;
                 index < this->rela_or_rel_plt_size / sizeof(Elf_Rela);
                 index++) {
                Elf_Rela *entry = &this->rela_or_rel_plt.rela[index];
                this->ResolveSymbolRelaJumpSlot(entry, do_lazy_got_init);
            }
        }
    } else if (this->rela_or_rel_plt_size >= sizeof(Elf_Rel)) {
        for (auto index = 0u;
             index < this->rela_or_rel_plt_size / sizeof(Elf_Rel); index++) {
            Elf_Rel *entry = &this->rela_or_rel_plt.rel[index];
            this->ResolveSymbolRelJumpSlot(entry, do_lazy_got_init);
        }
    }

    if (this->got) {
        this->got[1] = this;
        this->got[2] = (void *)__rtld_runtime_resolve;
    }
}

}  // namespace rtld