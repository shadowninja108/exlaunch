#pragma once

#include "util/ptr_path.hpp"
#include <common.hpp>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <bit>

namespace exl::util {

    template<typename R, typename... A>
    using GenericFuncPtr = R(*)(A...);

    namespace member_func {

        template<typename T, typename R, typename... A>
        using Ptr = R(T::*)(A...);

        template<typename T, typename R, typename... A>
        using ConstPtr = R(T::*)(A...) const;

        /* Itanium ABI. */
        template<typename T, typename R, typename... A>
        struct RichImpl {
            using Generic = GenericFuncPtr<R, T*, A...>;

            uintptr_t m_Ptr;
            ptrdiff_t m_Adj;
            
            constexpr bool IsVirtual() const {
                return (m_Ptr & 1) == 1;
            }

            constexpr Generic GetPtr(const T* ptr) const {
                if(IsVirtual()) {
                    /* Cast this pointer to something we can do arithmatic on. */
                    auto _this = reinterpret_cast<uintptr_t>(ptr);
                    /* m_Ptr is the vtable offset to call plus 1. */
                    ptrdiff_t offset = m_Ptr - 1;
                    /* Get the position of the vtable pointer within the structure. */
                    auto vtblptr = _this + m_Adj;
                    /* Dereference to get the vtable pointer. */
                    auto vtbl = *reinterpret_cast<std::byte**>(vtblptr);
                    /* Add the offset and cast to Type */
                    return *reinterpret_cast<Generic*>(vtbl + offset);
                } else {
                    /* m_Ptr is just the function pointer. */
                    return reinterpret_cast<Generic>(m_Ptr);
                }
            }

            template<typename... Args>
            ALWAYS_INLINE auto Call(T* _this, Args &&... args) const {
                return GetPtr(_this)(_this, std::forward<Args>(args)...);
            }
        };

        template<typename T>
        struct Rich;

        template<typename T, typename R, typename... A>
        struct Rich<Ptr<T, R, A...>> : public RichImpl<T, R, A...> {};

        template<typename T, typename R, typename... A>
        struct Rich<ConstPtr<T, R, A...>> : public RichImpl<const T, R, A...> {};

        template<typename T>
        static constexpr auto Adapt(T in) {
            return std::bit_cast<Rich<T>>(in);
        }
    }
}