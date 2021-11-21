#pragma once

#include "common.hpp"

#include "elf.hpp"

#include "rtld/ModuleHeader.hpp"
#include "rtld/ModuleList.hpp"
#include "rtld/ModuleObject.hpp"

using namespace rtld;

typedef Elf_Addr (*lookup_global_t)(const char *);

extern "C" void __rtld_runtime_resolve(void);

namespace rtld {
    Elf_Addr lookup_global_auto(const char *name);
}

namespace nn::ro::detail {
    extern ModuleObjectList g_pManualLoadList;
    extern ModuleObjectList g_pAutoLoadList;
    extern bool g_RoDebugFlag;
    extern lookup_global_t g_LookupGlobalManualFunctionPointer;
};

namespace ro = nn::ro::detail;

extern "C" {
    extern rtld::ModuleObject exl_nx_module_runtime;
};