#pragma once

#include "util/ptr_path.hpp"
#include "type_traits.hpp"
#include <common.hpp>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <bit>

namespace exl::util {

    template<typename R, typename... A>
    using CFuncPtr = R(*)(A...);

    template<typename R, typename... A>
    using CFuncPtrVariadic = R(*)(A..., ...);

    namespace member_func {

        template<typename T, typename R, typename... A>
        using Ptr = R(T::*)(A...);

        template<typename T, typename R, typename... A>
        using ConstPtr = R(T::*)(A...) const;

        /* ARM does this different from Itanium. https://github.com/ARM-software/abi-aa/blob/main/cppabi64/cppabi64.rst */
        template<typename T>
        requires util::FuncPtrTraits<T>::IsMemberFunc
        struct Rich {
            using Traits = FuncPtrTraits<T>;
            using CPtr = Traits::CPtr;

            uintptr_t m_Ptr;
            ptrdiff_t m_Adj;
            
            constexpr bool IsVirtual() const {
                /* "The least significant bit of adj then makes exactly the same discrimination as the least significant bit of ptr does for Itanium." */
                return (m_Adj & 1) == 1;
            }

            constexpr ptrdiff_t GetAdj() const {
                /* "adj contains twice the this adjustment, plus 1 if the member function is virtual." */
                auto diff = m_Adj;
                if(IsVirtual())
                    diff--;
                diff /= 2;
                return diff;
            }

            constexpr bool IsNull() const {
                /* "A pointer to member function is NULL when ptr = 0 and the least significant bit of adj is zero." */
                return m_Ptr == 0 && !IsVirtual();
            }

            constexpr auto AdjustThis(T* ptr) const {
                auto integer = reinterpret_cast<uintptr_t>(ptr);
                union {
                    uintptr_t m_Vtbl;
                    T m_Underlying;
                }* wrapped = reinterpret_cast<decltype(wrapped)>(integer + GetAdj());
                return wrapped;
            }

            constexpr CPtr GetPtr(T* ptr) const {
                if(IsVirtual()) {
                    /* Get vtable for ptr. */
                    auto vtbl = AdjustThis(ptr)->m_Vtbl;
                    /* Get entry within vtable. */
                    return *reinterpret_cast<CPtr*>(vtbl + m_Ptr);
                } else {
                    /* m_Ptr is just the function pointer. */
                    return reinterpret_cast<CPtr>(m_Ptr);
                }
            }

            template<typename... Args>
            ALWAYS_INLINE auto Call(T* _this, Args &&... args) const {
                return GetPtr(_this)(&AdjustThis(_this)->m_Underlying, std::forward<Args>(args)...);
            }
        };

        template<typename T>
        requires util::FuncPtrTraits<T>::IsMemberFunc
        static constexpr auto Adapt(T in) {
            return std::bit_cast<Rich<T>>(in);
        }
    }
}