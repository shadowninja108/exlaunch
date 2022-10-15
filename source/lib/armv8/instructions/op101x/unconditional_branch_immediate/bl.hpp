#pragma once

#include "base.hpp"

namespace exl::armv8::inst {

    struct BranchLink : public impl::op101x::UnconditionalBranchImmediate {

        constexpr BranchLink(uint relative_address) : UnconditionalBranchImmediate(UnconditionalBranchImmediate::BL, relative_address) {}
    };

    static_assert(BranchLink(0x4440).Value() == 0x94001110, "");
    static_assert(BranchLink(0x4200).Value() == 0x94001080, "");
    static_assert(BranchLink(0x6900).Value() == 0x94001A40, "");
    static_assert(BranchLink(0x0008).Value() == 0x94000002, "");
}