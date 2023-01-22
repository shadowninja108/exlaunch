#pragma once

#include "base.hpp"

namespace exl::armv8::inst {

    struct StrRegisterOffset : public impl::opx1x0::LoadStoreRegisterOffset {
        
        static constexpr u8 V = 0b0;
        static constexpr u8 Opc = 0b00;

        ACCESSOR(S,         12);
        ACCESSOR(Option,    13, 16);

        static constexpr u8 GetSize(reg::Register rt) {
            return 0b10 | rt.Is64();
        }

        /* TODO: merge */
        static constexpr u8 CreateOption(ExtendType extend) {
            switch(extend) {
                case ExtendType_UXTW:
                    return 0b010;
                case ExtendType_LSL:
                    return 0b011;
                case ExtendType_SXTW:
                    return 0b110;
                case ExtendType_SXTX:
                    return 0b111;
                default:
                    /* static_assert(false, ""); */
                    return 0b000;
            };
        }

        static constexpr bool CreateS(reg::Register rt, u8 amount) {
            if(amount == 0)
                return false;

            if(rt.Is64() && amount == 3)
                return true;
            
            if(rt.Is32() && amount == 2)
                return true;

            /* static_assert(false, ""); */
            return false;
        }

        constexpr StrRegisterOffset(reg::Register rt, reg::Register rn, reg::Register rm, ExtendType extend = ExtendType_LSL, u8 amount = 0) : LoadStoreRegisterOffset(GetSize(rt), V, Opc){
            /*
            static_assert(rt.Is64() == rm.Is64(), "");
            static_assert(extend & 0b010 != 0, "");
            static_assert(CreateOption(extend) == rm.Is64(), "");
            
            if(rt.Is64()) {
                static_assert(amount == 0 || amount == 3, "");
            } else {
                static_assert(amount == 0 || amount == 2, "");
            }
            */

            SetSize(GetSize(rt));
            SetRm(rm.Index());
            SetOption(CreateOption(extend));
            SetS(CreateS(rt, amount));
            SetRt(rt.Index());
            SetRn(rn.Index());
        }
        
        constexpr StrRegisterOffset(reg::Register rt, reg::Register rn, reg::Register rm, u8 amount) : StrRegisterOffset(rt, rn, rm, ExtendType_LSL, amount) {}
    };

    static_assert(StrRegisterOffset(reg::X0, reg::X1, reg::X2).Value()                      == 0xF8226820, "");
    static_assert(StrRegisterOffset(reg::X3, reg::X4, reg::X5).Value()                      == 0xF8256883, "");
    static_assert(StrRegisterOffset(reg::X6, reg::X7, reg::X8).Value()                      == 0xF82868E6, "");

    static_assert(StrRegisterOffset(reg::X0, reg::X1, reg::W2, ExtendType_UXTW, 3).Value()  == 0xF8225820, "");
    static_assert(StrRegisterOffset(reg::X3, reg::X4, reg::W5, ExtendType_UXTW, 3).Value()  == 0xF8255883, "");
    static_assert(StrRegisterOffset(reg::X6, reg::X7, reg::W8, ExtendType_UXTW, 3).Value()  == 0xF82858E6, "");

    static_assert(StrRegisterOffset(reg::X0, reg::X1, reg::X2, ExtendType_SXTX, 3).Value()  == 0xF822F820, "");
    static_assert(StrRegisterOffset(reg::X3, reg::X4, reg::X5, ExtendType_SXTX, 3).Value()  == 0xF825F883, "");
    static_assert(StrRegisterOffset(reg::X6, reg::X7, reg::X8, ExtendType_SXTX, 3).Value()  == 0xF828F8E6, "");
}