#include "common.hpp"

#include "program/setting.hpp"

struct ModuleName {
    int unknown;
    int name_length;
    char name[EXL_MODULE_NAME_LEN + 1];
};

__attribute__((section(".nx-module-name")))
const ModuleName s_ModuleName = {.unknown = 0, .name_length = EXL_MODULE_NAME_LEN, .name = EXL_MODULE_NAME};