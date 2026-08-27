#pragma once

#include <common.hpp>

#include "../../util/neon.hpp"

namespace exl::hook::nx64 {

    union GpRegister {
        u64 X;
        u32 W;
    };

    union GpRegisters {
        GpRegister m_Gp[31];
        struct {
            GpRegister _Gp[29];
            GpRegister m_Fp;
            GpRegister m_Lr;
        };
    };

    union Vec64 {
        util::neon::F64x1 D;
        util::neon::F32x2 S;
        util::neon::F16x4 H;
        /* TODO: 8-bit floats? That's a thing? */
        // F8x8 B;
    };

    union Vec128 {
        util::neon::F128x1 Q;
        util::neon::F64x2 D;
        util::neon::F32x4 S;
        util::neon::F16x8 H;
        /* TODO: 8-bit floats? That's a thing? */
        // F8x16 B;
    };

    union FloatRegister {
        Vec128 V128;
        f128 Q;
        Vec64 V64;
        f64 D;
        f32 S;
        f16 H;
        /* TODO: 8-bit floats? That's a thing? */
        // f8 B;
    };

    struct FloatRegisters {
        FloatRegister m_Fr[32];
    };

    namespace impl {
        /* This type is only unioned with GpRegisters, so this is valid. */
        struct GpRegisterAccessorImpl {
            GpRegisters& Get() {
                return *reinterpret_cast<GpRegisters*>(this);
            }
        };
        struct FloatRegisterAccessorImpl {
            FloatRegisters& Get() {
                return *reinterpret_cast<FloatRegisters*>(this);
            }
        };

        #define ACCESSOR(inherit, name, type, length, arr, member)                                          \
            struct name##RegisterAccessor : public inherit##Impl {                                          \
                type& operator[](int index) {                                                               \
                    EXL_ASSERT(0 <= index && index < length, "Register index must not be out of bounds!");  \
                    return Get().arr[index].member;                                                         \
                }                                                                                           \
            }
        
        ACCESSOR(GpRegisterAccessor,    Gp64,       u64,    31, m_Gp, X);
        ACCESSOR(GpRegisterAccessor,    Gp32,       u32,    31, m_Gp, W);
        ACCESSOR(FloatRegisterAccessor, Vector128,  Vec128, 32, m_Fr, V128);
        ACCESSOR(FloatRegisterAccessor, Vector64,   Vec64,  32, m_Fr, V64);
        ACCESSOR(FloatRegisterAccessor, Float128,   f128,   32, m_Fr, Q);
        ACCESSOR(FloatRegisterAccessor, Float64,    f64,    32, m_Fr, D);
        ACCESSOR(FloatRegisterAccessor, Float32,    f32,    32, m_Fr, S);
        ACCESSOR(FloatRegisterAccessor, Float16,    f16,    32, m_Fr, H);
        /* TODO: 8-bit floats? That's a thing? */
        //ACCESSOR(FloatRegisterAccessor, Float8,    f8,    32, m_Fr, B);

        #undef ACCESSOR
    }

    struct InlineCtx {
        union {
            /* Accessors are union'd with the gprs so that they can be accessed directly. */
            impl::Gp64RegisterAccessor X;
            impl::Gp32RegisterAccessor W;
            GpRegisters m_Gpr;
        };
    };

    struct InlineFloatCtx {
        /* Ensure the data is aligned for efficient access. */
        union ALIGNED(sizeof(FloatRegister)) {
            /* Accessors to simplify access to registers. */
            impl::Vector128RegisterAccessor V128;
            impl::Float128RegisterAccessor Q;
            impl::Vector64RegisterAccessor V64;
            impl::Float64RegisterAccessor D;
            impl::Float32RegisterAccessor S;
            FloatRegisters m_Fr;
        };
        union {
            /* Accessors are union'd with the gprs so that they can be accessed directly. */
            impl::Gp64RegisterAccessor X;
            impl::Gp32RegisterAccessor W;
            GpRegisters m_Gpr;
        };

        /* 
            This utility is meant to have closer parity to how vectors are referred to in assembly.
            Examples:
            `MOV V26.16H, V16.16H` would be `ctx->V<26, 16, 'H'>() = ctx->V<16, 16, 'H'>()`

            Pretty cursed, but I think this might make things easier for people who do not understand NEON (ie, people who touch grass).
        */
        template<size_t Index, size_t Length, char Type>
        ALWAYS_INLINE auto& V() {
            static_assert(0 <= Index && Index < 32, "Register index must not be out of bounds!");
            if constexpr(Length == 1) {
                if constexpr(Type == 'Q')
                    return V128[Index].Q;
                else if constexpr(Type == 'D')
                    return V64[Index].D;
                else static_assert(false, "Invalid type for 1 length vector!");
            } else if constexpr(Length == 2) {
                if constexpr(Type == 'D')
                    return V128[Index].D;
                else if constexpr(Type == 'S')
                    return V64[Index].S;
                else static_assert(false, "Invalid type for 2 length vector!");
            } else if constexpr(Length == 4) {
                if constexpr(Type == 'S')
                    return V128[Index].S;
                else if constexpr(Type == 'H')
                    return V64[Index].H;
                else static_assert(false, "Invalid type for 4 length vector!");
            } else if constexpr(Length == 8) {
                if constexpr(Type == 'H')
                    return V128[Index].H;
            /* TODO: 8-bit floats? That's a thing? */
            //  if constexpr(Type == 'B')
            //      return V64[Index].B;
                else static_assert(false, "Invalid type for 8 length vector!");
            // } else if constexpr(Length == 16) {
            //     if constexpr(Type == 'B')
            //         return V64[Index].B;
            //     else static_assert(false, "Invalid type for 16 length vector!");
            } else static_assert(false, "Invalid vector length!");
        }
    };

    void InitializeInline();
}