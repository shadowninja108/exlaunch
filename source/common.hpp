/*
 * Copyright (c) Atmosphère-NX
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "types.h"

#include "lib/alloc.hpp"
#include "lib/nx/nx.h"
#include "lib/result.hpp"
#include "lib/libsetting.hpp"

#include <stdfloat>

using f16 = std::float16_t;
using f128 = std::float128_t;

#define APPEND_IMPL(x, y) x ## y
#define APPEND(x, y) APPEND_IMPL(x, y)

#define NON_COPYABLE(cls) \
    cls(const cls&) = delete; \
    cls& operator=(const cls&) = delete

#define NON_MOVEABLE(cls) \
    cls(cls&&) = delete; \
    cls& operator=(cls&&) = delete

#define NUM_ARGS_(_1, _2, _3, _4, _5, _6, TOTAL, ...) TOTAL
#define NUM_ARGS(...) NUM_ARGS_(__VA_ARGS__, 6, 5, 4, 3, 2, 1)
#define VA_MACRO(MACRO, ...) APPEND(MACRO, NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

namespace exl::impl {

    template<typename... ArgTypes>
    constexpr ALWAYS_INLINE void UnusedImpl(ArgTypes &&... args) {
        (static_cast<void>(args), ...);
    }

}

#define EXL_UNUSED(...) ::exl::impl::UnusedImpl(__VA_ARGS__)

#define EXL_PREDICT(expr, value, _probability) __builtin_expect_with_probability(expr, value, ({ \
                                                    constexpr double probability = _probability; \
                                                    static_assert(0.0 <= probability);           \
                                                    static_assert(probability <= 1.0);           \
                                                    probability;                                 \
                                               }))

#define EXL_PREDICT_TRUE(expr, probability)  EXL_PREDICT(!!(expr), 1, probability)
#define EXL_PREDICT_FALSE(expr, probability) EXL_PREDICT(!!(expr), 0, probability)

#define EXL_LIKELY(expr)   EXL_PREDICT_TRUE(expr, 1.0)
#define EXL_UNLIKELY(expr) EXL_PREDICT_FALSE(expr, 1.0)

#define EXL_ASSUME(expr) do { if (!static_cast<bool>((expr))) { __builtin_unreachable(); } } while (0)
