#include "utils.hpp"

extern "C" unsigned long __rtld_elf_hash(const char *name) {
    unsigned long h = 0;
    unsigned long g;

    while (*name) {
        h = (h << 4) + *name++;
        if ((g = h & 0xf0000000)) h ^= g >> 24;
        h &= ~g;
    }
    return h;
}