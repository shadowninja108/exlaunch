#pragma once

#include "base.hpp"

#include <bit>
#include <algorithm>

namespace exl::armv8::inst {

    struct OrrImmediate : public impl::op100x::LogicalImmediate {
        
        static constexpr u8 Opc = 0b01;

        static constexpr auto ImmBase = 10;

        static constexpr auto ImmsLow = 10;
        static constexpr auto ImmsHigh = 16;

        static constexpr auto ImmrLow = 16;
        static constexpr auto ImmrHigh = 20;

        static constexpr auto NLow = 22;

        ACCESSOR(N,     NLow);
        ACCESSOR(Immr,  ImmrLow, ImmrHigh);
        ACCESSOR(Imms,  ImmsLow, ImmsHigh);
        ACCESSOR(Rn,    5, 10);
        ACCESSOR(Rd,    0, 5);

        using ImmType = u16;

        static constexpr auto RMaskForImm  = util::Mask<ImmType, ImmrLow - ImmBase, ImmrHigh - ImmBase>();
        static constexpr auto SMaskForImm  = util::Mask<ImmType, ImmsLow - ImmBase, ImmsHigh - ImmBase>();
        static constexpr auto NMaskForImm  = util::Mask<ImmType, NLow - ImmBase>();

        static constexpr util::BitSet<ImmType> EncodeImmediate(ImmType imm) {
            /*
                static_assert(imm != 0);
            */

            /* Compute the needed rotations. */
            auto r = static_cast<u32>(std::countr_zero(imm));
            /* Compute the immediate to encode. */
            u32 immShift = imm >> r;
            /* Compute the amount of bits needed to encode the immediate. */
            ImmType width = std::bit_width(immShift);
            /* Round up the width to a power of 2. */
            ImmType widthCeil = std::clamp(std::bit_ceil(width), static_cast<ImmType>(2), static_cast<ImmType>(64));

            /* N should be 64 if the total value is 64 bits. */
            bool n = widthCeil == 64;

            /* Compute mask where the width will fit in. */
            auto widthMask = static_cast<u32>(std::bit_width(imm) << 1) - 1;

            /* Initialize s and carve out space for the width and extra 0 bit. */
            auto s = ((1u << ImmsMask.Count) - 1u) & ~widthMask;
            /* OR in the width. */
            s |= (width - 1) & widthMask;

            /* Build up BitSet to return. */
            auto b = util::BitSet<ImmType>();
            b.SetBits<RMaskForImm>(r);
            b.SetBits<SMaskForImm>(s);
            b.SetBits<NMaskForImm>(n);
            return b;
        } 


        constexpr OrrImmediate(reg::Register rd, reg::Register rn, ImmType imm) : LogicalImmediate(rd.Is64(), Opc) {
            auto immb = EncodeImmediate(imm);
            
            /* static_asert(rd.Is64() == rn.Is64(), ""); */
            /* static_assert(rd.Is32() != immb.BitsOf<ImmMask3>(), ""); */

            SetN(immb.BitsOf<NMaskForImm>());
            SetImmr(immb.BitsOf<RMaskForImm>());
            SetImms(immb.BitsOf<SMaskForImm>());
            SetRn(rn.Index());
            SetRd(rd.Index());
        }

    };

    //static constexpr auto Test = OrrImmediate(reg::X0, reg::X1, 0b11111);

    //static_assert(Test.Value()   == 0xB2401020, "");
    //static_assert(OrrImmediate(reg::W2, reg::W3, 0b111100).Value()  == 0x321E0C62, "");
    //static_assert(OrrImmediate(reg::X4, reg::X5, 0b111).Value()     == 0xB24008A4, "");
}