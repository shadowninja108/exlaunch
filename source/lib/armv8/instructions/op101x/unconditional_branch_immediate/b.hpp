#pragma once

#include "base.hpp"

namespace exl::armv8::inst {

    struct Branch : public impl::op101x::UnconditionalBranchImmediate {
        
        constexpr Branch(uint relative_address) : UnconditionalBranchImmediate(UnconditionalBranchImmediate::B, relative_address) {}
    };
}