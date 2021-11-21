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