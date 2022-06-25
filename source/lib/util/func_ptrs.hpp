#pragma once

#include <common.hpp>

namespace exl::util {

    template<typename R, typename... A>
    using GenericFuncPtr = R(*)(A...);
    
}