#pragma once

#include "base.hpp"

namespace exl::armv8::inst {

    struct BranchLink : public impl::op101x::UnconditionalBranchImmediate {

        constexpr BranchLink(uint relative_address) : UnconditionalBranchImmediate(UnconditionalBranchImmediate::BL, relative_address) {}
    };
}