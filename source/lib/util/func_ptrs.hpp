#pragma once

#include <common.hpp>

namespace exl::util {

    template<typename R, typename... A>
    using GenericFuncPtr = R(*)(A...);
    
    template<typename T, typename R, typename... A>
    using MemberFuncPtr = R(T::*)(A...);
}