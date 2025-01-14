#pragma once

#include <common.hpp>

namespace exl::util::neon {

    #define VECTOR_SIZE(size) [[gnu::vector_size(size)]]
    #define VECTOR_NAME(bits, length) F##bits##x##length
    #define VECTOR_TYPE(bits, length)                                                                           \
        using VECTOR_NAME(bits, length) = f##bits VECTOR_SIZE(sizeof(f##bits) * length);                        \
        static_assert(sizeof(VECTOR_NAME(bits, length)) == sizeof(f##bits) * length,  STRINGIFY(VECTOR_NAME(bits, length))  " type is wrong!")

    VECTOR_TYPE(16, 4);
    VECTOR_TYPE(32, 2);
    VECTOR_TYPE(64, 1);

    VECTOR_TYPE(16, 8);
    VECTOR_TYPE(32, 4);
    VECTOR_TYPE(64, 2);
    VECTOR_TYPE(128, 1);

    template<char Type>
    struct TypeCharAdapter { static_assert(false, "Invalid float type!"); };

    template<>
    struct TypeCharAdapter<'Q'> { using Type = f128; };
    template<>
    struct TypeCharAdapter<'D'> { using Type = f64; };
    template<>
    struct TypeCharAdapter<'S'> { using Type = f32; };
    template<>
    struct TypeCharAdapter<'H'> { using Type = f16; };

    /* This does not work with standard notation on GCC, strangely. Seems to be a bug from 2019. */
    template<size_t Length, char Type>
    using VectorAdapter VECTOR_SIZE(sizeof(TypeCharAdapter<Type>::Type) * Length) = TypeCharAdapter<Type>::Type;

    #undef VECTOR_SIZE
    #undef VECTOR_NAME
    #undef VECTOR_TYPE
}