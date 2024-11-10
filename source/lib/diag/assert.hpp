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

// #include "abort.hpp"
#include "program/setting.hpp"
#include <cstdarg>
#include <type_traits>

namespace exl {

    using Result = ::Result;

    namespace os {

        struct UserExceptionInfo;

    }

    namespace impl {

        NORETURN void UnexpectedDefaultImpl(const char *func, const char *file, int line);

    }

}

namespace exl::diag {

    enum AssertionType {
        AssertionType_Audit,
        AssertionType_Assert,
    };

    struct LogMessage;

    struct AssertionInfo {
        AssertionType type;
        const LogMessage *message;
        const char *expr;
        const char *func;
        const char *file;
        int line;
    };

    enum AbortReason {
        AbortReason_Audit,
        AbortReason_Assert,
        AbortReason_Abort,
        AbortReason_UnexpectedDefault,
    };

    struct AbortInfo {
        AbortReason reason;
        const LogMessage *message;
        const char *expr;
        const char *func;
        const char *file;
        int line;
    };

    void OnAssertionFailure(AssertionType type, const char *expr, const char *func, const char *file, int line, const char *format, ...) __attribute__((format(printf, 6, 7)));
    void OnAssertionFailure(AssertionType type, const char *expr, const char *func, const char *file, int line);

    NORETURN void AbortImpl(const char *expr, const char *func, const char *file, int line);
    NORETURN void AbortImpl(const char *expr, const char *func, const char *file, int line, const char *format, ...) __attribute__((format(printf, 5, 6)));
    NORETURN void AbortImpl(const char *expr, const char *func, const char *file, int line, const ::exl::Result *result, const char *format, ...) __attribute__((format(printf, 6, 7)));

    NORETURN void AbortImpl(const char *expr, const char *func, const char *file, int line, const ::exl::Result *result, const ::exl::os::UserExceptionInfo *exception_info, const char *fmt, ...) __attribute__((format(printf, 7, 8)));

    NORETURN void VAbortImpl(const char *expr, const char *func, const char *file, int line, const ::exl::Result *result, const ::exl::os::UserExceptionInfo *exception_info, const char *fmt, std::va_list vl);

}

#ifdef EXL_DEBUG
#define EXL_CALL_ASSERT_FAIL_IMPL(type, expr, ...) ::exl::diag::OnAssertionFailure(type, expr, __PRETTY_FUNCTION__, __FILE__, __LINE__, ## __VA_ARGS__)
#define EXL_CALL_ABORT_IMPL(expr, ...)       ::exl::diag::AbortImpl(expr, __PRETTY_FUNCTION__, __FILE__, __LINE__, ## __VA_ARGS__)
#define EXL_UNREACHABLE_DEFAULT_CASE() default: ::exl::impl::UnexpectedDefaultImpl(__PRETTY_FUNCTION__, __FILE__, __LINE__)
#else
#define EXL_CALL_ASSERT_FAIL_IMPL(type, expr, ...) ::exl::diag::OnAssertionFailure(type, "", "", "", 0)
#define EXL_CALL_ABORT_IMPL(expr, ...)       ::exl::diag::AbortImpl("", "", "", 0); EXL_UNUSED(expr, ## __VA_ARGS__)
#define EXL_UNREACHABLE_DEFAULT_CASE() default: ::exl::impl::UnexpectedDefaultImpl("", "", 0)
#endif

#ifdef EXL_DEBUG
#define EXL_ASSERT_IMPL(type, expr, ...)                                                              \
    {                                                                                                 \
        if (std::is_constant_evaluated()) {                                                           \
            EXL_ASSUME(static_cast<bool>(expr));                                                      \
        } else {                                                                                      \
            if (const bool __tmp_exlassert_val = static_cast<bool>(expr); (!__tmp_exlassert_val)) { \
                EXL_CALL_ASSERT_FAIL_IMPL(type, #expr, ## __VA_ARGS__);                               \
            }                                                                                         \
        }                                                                                             \
    }
#else
#define EXL_ASSERT_IMPL(type, expr, ...) EXL_UNUSED(expr, ## __VA_ARGS__)
#endif

#define EXL_ASSERT(expr, ...) EXL_ASSERT_IMPL(::exl::diag::AssertionType_Assert, expr, ## __VA_ARGS__)

#define EXL_ABORT(...) EXL_CALL_ABORT_IMPL("", ## __VA_ARGS__)

#define EXL_ABORT_UNLESS(expr, ...)                                                                               \
    {                                                                                                             \
        if (std::is_constant_evaluated()) {                                                                       \
            EXL_ASSUME(static_cast<bool>(expr));                                                                  \
        } else {                                                                                                  \
            if (const bool __tmp_exlassert_val = static_cast<bool>(expr); EXL_UNLIKELY(!__tmp_exlassert_val)) { \
                EXL_CALL_ABORT_IMPL(#expr, ##__VA_ARGS__);                                                        \
            }                                                                                                     \
        }                                                                                                         \
    }
