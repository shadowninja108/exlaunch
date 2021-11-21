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

#include "abort.hpp"

#define EXL_CALL_ASSERT_FAIL_IMPL(cond, ...) ::exl::diag::AssertionFailureImpl(__FILE__, __LINE__, __PRETTY_FUNCTION__, cond, 0, ## __VA_ARGS__)
#define EXL_CALL_ABORT_IMPL(cond, value, ...)  ::exl::diag::AbortImpl(__FILE__, __LINE__, __PRETTY_FUNCTION__, cond, value, ## __VA_ARGS__)

#define EXL_UNREACHABLE_DEFAULT_CASE() default: EXL_CALL_ABORT_IMPL("Unreachable default case entered", 0)
#define EXL_ABORT(value, ...) EXL_CALL_ABORT_IMPL("", value, ## __VA_ARGS__)


#define EXL_ASSERT_IMPL(expr, ...)                                                                            \
    ({                                                                                                        \
        if (!(static_cast<bool>(expr))) { \
            EXL_CALL_ASSERT_FAIL_IMPL(#expr, ## __VA_ARGS__);                                                 \
        }                                                                                                     \
    })

#define EXL_ASSERT(expr, ...) EXL_ASSERT_IMPL(expr, ## __VA_ARGS__)