#include "lib/reloc/rtld.hpp"
#include "utils.hpp"

Elf_Addr rtld::lookup_global_auto(const char *name) {
    if (ro::g_pAutoLoadList.back == (ModuleObject *)&ro::g_pAutoLoadList) {
        return 0;
    }

    for (ModuleObject *module : ro::g_pAutoLoadList) {
        Elf_Sym *symbol = module->GetSymbolByName(name);
        if (symbol && ELF_ST_BIND(symbol->st_info)) {
            return (Elf_Addr)module->module_base + symbol->st_value;
        }
    }
    return 0;
}

extern "C" Elf_Addr __rtld_lazy_bind_symbol(ModuleObject *module,
                                            size_t index) {
    if (module->is_rela) {
        Elf_Rela *entry = &module->rela_or_rel_plt.rela[index];
        Elf_Sym *symbol = &module->dynsym[ELF_R_SYM(entry->r_info)];

        Elf_Addr target_symbol_address;

        if (module->TryResolveSymbol(&target_symbol_address, symbol)) {
            if (target_symbol_address == 0) {
                return 0;
            }

            return target_symbol_address + entry->r_addend;
        } else {
            print_unresolved_symbol(&module->dynstr[symbol->st_name]);
        }
    } else {
        Elf_Rel *entry = &module->rela_or_rel_plt.rel[index];
        Elf_Sym *symbol = &module->dynsym[ELF_R_SYM(entry->r_info)];

        Elf_Addr target_symbol_address;

        if (module->TryResolveSymbol(&target_symbol_address, symbol)) {
            return target_symbol_address;
        } else {
            print_unresolved_symbol(&module->dynstr[symbol->st_name]);
        }
    }

    return 0;
}