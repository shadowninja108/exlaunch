#include "common.hpp"

#include "program/setting.hpp"
#include <string>

constexpr const int ModuleNameLength = std::char_traits<char>::length(EXL_MODULE_NAME);

struct ModuleName {
    int unknown;
    int name_length;
    char name[ModuleNameLength + 1];
};

__attribute__((section(".nx-module-name")))
const ModuleName s_ModuleName = {.unknown = 0, .name_length = ModuleNameLength, .name = EXL_MODULE_NAME};
