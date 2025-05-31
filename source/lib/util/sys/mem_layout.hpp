#pragma once

#include <common.hpp>
#include <lib/util/module_index.hpp>
#include <array>
#include <bitset>
#include <rtld.hpp>

#include "module_info.hpp"

namespace exl::util {

    namespace mem_layout {
        inline Range s_Alias;
        inline Range s_Heap;
        inline Range s_Aslr;
        inline Range s_Stack;
    }

    namespace impl {
        void InitMemLayout();

        namespace mem_layout {
            extern std::array<ModuleInfo, static_cast<int>(ModuleIndex::End)> s_ModuleInfos;
            extern std::bitset<static_cast<int>(ModuleIndex::End)> s_ModuleBitset;
        }
    }

    [[gnu::const]] inline bool HasModule(ModuleIndex index) {
        return impl::mem_layout::s_ModuleBitset[static_cast<int>(index)];
    }

    [[gnu::const]] inline const ModuleInfo& GetModuleInfo(ModuleIndex index) {
        EXL_ABORT_UNLESS(HasModule(index));
        return impl::mem_layout::s_ModuleInfos[static_cast<int>(index)];
    }
}