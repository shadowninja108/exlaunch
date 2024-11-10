#pragma once

namespace exl::util {
    template<typename T>
    struct FuncPtrTraits {
        static_assert(false, "Unsupported type.");
    };

    /* Regular C function pointer. */
    template<typename R, typename... Args>
    struct FuncPtrTraits<R (*)(Args...)> {
        using Ptr = R (*)(Args...);
        using CPtr = R (*)(Args...);
        using ReturnType = R;

        static constexpr bool IsVariadic = false;
        static constexpr bool IsMemberFunc = false;
    };

    /* Regular C function pointer with variadic args. */
    template<typename R, typename... Args>
    struct FuncPtrTraits<R (*)(Args..., ...)> {
        using Ptr = R (*)(Args..., ...);
        using CPtr = R (*)(Args..., ...);
        using ReturnType = R;

        static constexpr bool IsVariadic = true;
        static constexpr bool IsMemberFunc = false;
    };

    /* C++ function pointer with non-const T pointer. */
    template<typename R, typename T, typename... Args>
    struct FuncPtrTraits<R (T::*)(Args...)> {
        using Ptr = R (T::*)(Args...);
        using CPtr = R (*)(T*, Args...);
        using ReturnType = R;
        using ThisType = T;

        static constexpr bool IsVariadic = false;
        static constexpr bool IsMemberFunc = true;
    };

    /* C++ function pointer with const T pointer. */
    template<typename R, typename T, typename... Args>
    struct FuncPtrTraits<R (T::*)(Args...) const> {
        using Ptr = R (T::*)(Args...) const;
        using CPtr = R (*)(const T*, Args...);
        using ReturnType = R;
        using ThisType = const T;

        static constexpr bool IsVariadic = false;
        static constexpr bool IsMemberFunc = true;
    };
    /* C++ function pointer with non-const T pointer and variadic args. */
    template<typename R, typename T, typename... Args>
    struct FuncPtrTraits<R (T::*)(Args..., ...)> {
        using Ptr = R (T::*)(Args..., ...);
        using CPtr = R (*)(T*, Args..., ...);
        using ReturnType = R;
        using ThisType = T;

        static constexpr bool IsVariadic = true;
        static constexpr bool IsMemberFunc = true;
    };

    /* C++ function pointer with const T pointer and variadic args. */
    template<typename R, typename T, typename... Args>
    struct FuncPtrTraits<R (T::*)(Args..., ...) const> {
        using Ptr = R (T::*)(Args..., ...) const;
        using CPtr = R (*)(const T*, Args..., ...);
        using ReturnType = R;
        using ThisType = const T;

        static constexpr bool IsVariadic = true;
        static constexpr bool IsMemberFunc = true;
    };

    /* TODO: more variations as I can be assed to support them. volatile? noexcept? Do I care? Somebody probably does. */
}