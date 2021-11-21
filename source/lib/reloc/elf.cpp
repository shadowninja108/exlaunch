#include "elf.hpp"

#include "common.hpp"

#include "rtld.hpp"

extern "C" {
    __attribute__((section(".bss")))
    rtld::ModuleObject exl_nx_module_runtime;

    void exl_dynamic(uintptr_t aslr_base, const Elf_Dyn* dynamic)
    {
        Elf_Addr rela = 0;
        Elf_Addr rel = 0;

        Elf_Xword rela_entry_size = sizeof(Elf_Rela);
        Elf_Xword rel_entry_size = sizeof(Elf_Rel);

        Elf_Xword rela_entry_count = 0;
        Elf_Xword rel_entry_count = 0;

        Elf_Xword rela_size = 0;
        Elf_Xword rel_size = 0;

        for (; dynamic->d_tag != DT_NULL; dynamic++) {
            switch (dynamic->d_tag) {
                case DT_RELA:
                    rela = ((Elf_Addr)aslr_base + dynamic->d_un.d_ptr);
                    continue;

                case DT_RELAENT:
                    rela_entry_size = dynamic->d_un.d_val;
                    continue;

                case DT_RELASZ:
                    rela_size = dynamic->d_un.d_val;
                    continue;

                case DT_REL:
                    rel = ((Elf_Addr)aslr_base + dynamic->d_un.d_ptr);
                    continue;

                case DT_RELENT:
                    rel_entry_size = dynamic->d_un.d_val;
                    continue;

                case DT_RELSZ:
                    rel_size = dynamic->d_un.d_val;
                    continue;

                case DT_RELACOUNT:
                    rela_entry_count = dynamic->d_un.d_val;
                    continue;

                case DT_RELCOUNT:
                    rel_entry_count = dynamic->d_un.d_val;
                    continue;

                // those are nop on the real rtld
                case DT_NEEDED:
                case DT_PLTRELSZ:
                case DT_PLTGOT:
                case DT_HASH:
                case DT_STRTAB:
                case DT_SYMTAB:
                case DT_STRSZ:
                case DT_SYMENT:
                case DT_INIT:
                case DT_FINI:
                case DT_SONAME:
                case DT_RPATH:
                case DT_SYMBOLIC:
                default:
                    continue;
            }
        }
        
        if(rela_entry_count == 0)
            rela_entry_count = rela_size / rela_entry_size;
        if(rel_entry_count == 0)
            rel_entry_count = rel_size / rel_entry_size;


        if (rel_entry_count) {
            Elf_Xword i = 0;

            while (i < rel_entry_count) {
                Elf_Rel *entry = (Elf_Rel *)(rel + (i * rel_entry_size));
                switch (ELF_R_TYPE(entry->r_info)) {
                    case ARCH_RELATIVE: {
                        Elf_Addr *ptr = (Elf_Addr *)(aslr_base + entry->r_offset);
                        *ptr += (Elf_Addr)aslr_base;
                        break;
                    }
                }
                i++;
            }
        }

        if (rela_entry_count) {
            Elf_Xword i = 0;

            while (i < rela_entry_count) {
                Elf_Rela *entry = (Elf_Rela *)(rela + (i * rela_entry_size));

                switch (ELF_R_TYPE(entry->r_info)) {
                    case ARCH_RELATIVE: {
                        Elf_Addr *ptr = (Elf_Addr *)(aslr_base + entry->r_offset);
                        *ptr = (Elf_Addr)aslr_base + entry->r_addend;
                        break;
                    }
                }
                i++;
            }
        }
    }
};