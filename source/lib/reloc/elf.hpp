#pragma once

#include <elf.h>

/* TODO: 32-bit support? */
typedef Elf64_Addr Elf_Addr;
typedef Elf64_Rel Elf_Rel;
typedef Elf64_Rela Elf_Rela;
typedef Elf64_Dyn Elf_Dyn;
typedef Elf64_Sym Elf_Sym;
typedef Elf64_Xword Elf_Xword;

#define ELF_R_SYM ELF64_R_SYM
#define ELF_R_TYPE ELF64_R_TYPE
#define ELF_ST_BIND ELF64_ST_BIND
#define ELF_ST_VISIBILITY ELF64_ST_VISIBILITY

#define ARCH_RELATIVE R_AARCH64_RELATIVE
#define ARCH_JUMP_SLOT R_AARCH64_JUMP_SLOT
#define ARCH_GLOB_DAT R_AARCH64_GLOB_DAT
#define ARCH_IS_REL_ABSOLUTE(type) \
    type == R_AARCH64_ABS32 || type == R_AARCH64_ABS64
