#pragma once

#include "../armv8.hpp"

#define _ACCESSOR_MASK_NAME(name)   \
    APPEND(name, Mask)
#define _ACCESSOR_GETTER_NAME(name) \
    Get##name
#define _ACCESSOR_SETTER_NAME(name) \
    Set##name


#define _ACCESSOR_BODY(name)                                    \
    constexpr InstType _ACCESSOR_GETTER_NAME(name)() const {    \
        return BitsOf<_ACCESSOR_MASK_NAME(name)>();             \
    }                                                           \
    constexpr void _ACCESSOR_SETTER_NAME(name)(InstType val) {  \
        SetBits<_ACCESSOR_MASK_NAME(name)>(val);                \
    }

#define _ACCESSOR_2(name, low)                                          \
    static constexpr auto _ACCESSOR_MASK_NAME(name) = InstMask<low>();  \
    _ACCESSOR_BODY(name)
#define _ACCESSOR_3(name, low, high)                                            \
    static constexpr auto _ACCESSOR_MASK_NAME(name) = InstMask<low, high>();    \
    _ACCESSOR_BODY(name)

#define ACCESSOR(...) VA_MACRO(_ACCESSOR_, __VA_ARGS__)

#include "instructions/base.hpp"

#undef _ACCESSOR_MASK_NAME
#undef _ACCESSOR_GETTER_NAME
#undef _ACCESSOR_SETTER_NAME
#undef _ACCESSOR_BODY
#undef _ACCESSOR_2
#undef _ACCESSOR_3
#undef ACCESSOR