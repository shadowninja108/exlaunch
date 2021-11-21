#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../elf.hpp"

namespace rtld {

struct ModuleObject {

   private:
    // ResolveSymbols internals
    inline void ResolveSymbolRelAbsolute(Elf_Rel *entry);
    inline void ResolveSymbolRelaAbsolute(Elf_Rela *entry);
    inline void ResolveSymbolRelJumpSlot(Elf_Rel *entry, bool do_lazy_got_init);
    inline void ResolveSymbolRelaJumpSlot(Elf_Rela *entry,
                                          bool do_lazy_got_init);

   public:
    struct ModuleObject *next;
    struct ModuleObject *prev;
    union {
        Elf_Rel *rel;
        Elf_Rela *rela;
        void *raw;
    } rela_or_rel_plt;
    union {
        Elf_Rel *rel;
        Elf_Rela *rela;
    } rela_or_rel;
    char *module_base;
    Elf_Dyn *dynamic;
    bool is_rela;
    Elf_Xword rela_or_rel_plt_size;
    void (*dt_init)(void);
    void (*dt_fini)(void);
    uint32_t *hash_bucket;
    uint32_t *hash_chain;
    char *dynstr;
    Elf_Sym *dynsym;
    Elf_Xword dynstr_size;
    void **got;
    Elf_Xword rela_dyn_size;
    Elf_Xword rel_dyn_size;
    Elf_Xword rel_count;
    Elf_Xword rela_count;
    Elf_Xword hash_nchain_value;
    Elf_Xword hash_nbucket_value;
    void *got_stub_ptr;
#ifdef __RTLD_6XX__
    Elf_Xword soname_idx;
    size_t nro_size;
    bool cannot_revert_symbols;
#endif

    void Initialize(char *aslr_base, Elf_Dyn *dynamic);
    void Relocate();
    Elf_Sym *GetSymbolByName(const char *name);
    void ResolveSymbols(bool do_lazy_got_init);
    bool TryResolveSymbol(Elf_Addr *target_symbol_address, Elf_Sym *symbol);
};

#ifdef __RTLD_6XX__
#ifdef __aarch64__
static_assert(sizeof(ModuleObject) == 0xD0, "ModuleObject size isn't valid");
#elif __arm__
static_assert(sizeof(ModuleObject) == 0x68, "ModuleObject size isn't valid");
#endif
#else
#ifdef __aarch64__
static_assert(sizeof(ModuleObject) == 0xB8, "ModuleObject size isn't valid");
#elif __arm__
static_assert(sizeof(ModuleObject) == 0x5C, "ModuleObject size isn't valid");
#endif
#endif
}  // namespace rtld