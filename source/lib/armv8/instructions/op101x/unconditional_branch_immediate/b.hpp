#pragma once

#include "base.hpp"

namespace exl::armv8::inst {

    struct Branch : public impl::op101x::UnconditionalBranchImmediate {
        
        constexpr Branch(uint relative_address) : UnconditionalBranchImmediate(UnconditionalBranchImmediate::B, relative_address) {}
    };

    static_assert(Branch(0x4440).Value() == 0x14001110, "");
    static_assert(Branch(0x4200).Value() == 0x14001080, "");
    static_assert(Branch(0x6900).Value() == 0x14001A40, "");
    static_assert(Branch(0x0008).Value() == 0x14000002, "");
}