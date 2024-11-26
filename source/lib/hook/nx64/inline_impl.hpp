#pragma once

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

    union Vec {
        struct {
            f64 D1;
            f64 D2;
        };
        struct {
            f32 S1;
            f32 S2;
            f32 S3;
            f32 S4;
        };
    };

    union FloatRegister {
        Vec V;
        f64 D;
        f32 S;
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

        struct GpRegisterAccessor64 : public GpRegisterAccessorImpl {
            u64& operator[](int index)
            {
                return Get().m_Gp[index].X;
            }
        };

        struct GpRegisterAccessor32 : public GpRegisterAccessorImpl {
            u32& operator[](int index)
            {
                return Get().m_Gp[index].W;
            }
        };

        struct FloatRegisterAccessorImpl {
            FloatRegisters& Get() {
                return *reinterpret_cast<FloatRegisters*>(this);
            }
        };

        struct FloatRegisterAccessor128 : public FloatRegisterAccessorImpl {
            Vec& operator[](int index) {
                EXL_ASSERT(index >= 0 && index < 32, "Register index must not be out of bounds!");
                return Get().m_Fr[index].V;
            }
        };

        struct FloatRegisterAccessor64 : public FloatRegisterAccessorImpl {
            f64& operator[](int index) {
                EXL_ASSERT(index >= 0 && index < 32, "Register index must not be out of bounds!");
                return Get().m_Fr[index].D;
            }
        };

        struct FloatRegisterAccessor32 : public FloatRegisterAccessorImpl {
            f32& operator[](int index) {
                EXL_ASSERT(index >= 0 && index < 32, "Register index must not be out of bounds!");
                return Get().m_Fr[index].S;
            }
        };
    }

    struct InlineCtx {
        union {
            /* Accessors are union'd with the gprs so that they can be accessed directly. */
            impl::GpRegisterAccessor64 X;
            impl::GpRegisterAccessor32 W;
            GpRegisters m_Gpr;
        };
    };

    struct InlineFloatCtx : InlineCtx {
        union {
            impl::FloatRegisterAccessor128 V;
            impl::FloatRegisterAccessor64 D;
            impl::FloatRegisterAccessor32 S;
            FloatRegisters m_Fr[32];
        };
    };

    void InitializeInline();
}