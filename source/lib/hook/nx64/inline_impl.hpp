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
    }

    struct InlineCtx {
        union {
            /* Accessors are union'd with the gprs so that they can be accessed directly. */
            impl::GpRegisterAccessor64 X;
            impl::GpRegisterAccessor32 W;
            GpRegisters m_Gpr;
        };
    };

    void InitializeInline();
}