#pragma once

#include <common.hpp>
#include <concepts>

namespace exl::util {
    
    template<
        std::integral Underlying, 
        Underlying _Low,
        Underlying _High = _Low + 1>
    struct Mask {
        static constexpr Underlying Low = _Low;
        static constexpr Underlying High = _High;
        static constexpr Underlying Count = _High - _Low;
        static constexpr Underlying Value() {
            auto base = (1 << Count) - 1;
            return base << Low;
        }
    };

    template<std::integral Underlying>
    class BitSet {
        private:
        Underlying m_Data;

        public:
        constexpr inline BitSet() : m_Data() {}
        constexpr inline BitSet(Underlying data) : m_Data(data) {}

        template<Mask Mask>
        constexpr Underlying inline BitsOf() const {
            /* Take out the bits we want. */
            auto value = m_Data & Mask.Value();
            /* Shift down the bits. */
            return value >> Mask.Low;
        }

        template<Mask Mask>
        constexpr void inline SetBits(Underlying value) {
            /* Carve out the bits not in the mask. */
            m_Data &= ~Mask.Value();

            /* Prepare value to be written. */
            auto v = value << Mask.Low;
            v &= Mask.Value();

            /* OR in the bits. */
            m_Data |= v;
        }

        /* Wrappers to construct masks. */
        template<Underlying Low, Underlying High>
        constexpr Underlying inline BitsOf() const {
            return BitsOf<Mask<Underlying, Low, High>>();
        }
        template<Underlying Low, Underlying High>
        constexpr void inline SetBits(Underlying value) {
            SetBits<Mask<Underlying, Low, High>>(value);
        }
        
        /* Conversion operators. */
        constexpr inline Underlying& operator=(const Underlying& value) { m_Data = value; return this; }

        constexpr inline Underlying Value() const { return m_Data; }
    };
};