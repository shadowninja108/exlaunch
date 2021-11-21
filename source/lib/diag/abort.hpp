#pragma once

#include "common.hpp"

namespace exl::diag {

    struct AbortCtx {
        Result m_Result;
    };

    void NORETURN NOINLINE AssertionFailureImpl(const char *file, int line, const char *func, const char *expr, u64 value, const char *format, ...) __attribute__((format(printf, 6, 7)));
    void NORETURN NOINLINE AssertionFailureImpl(const char *file, int line, const char *func, const char *expr, u64 value);

    void NORETURN NOINLINE AbortImpl(const char *file, int line, const char *func, const char *expr, u64 value, const char *format, ...) __attribute__((format(printf, 6, 7)));
    void NORETURN NOINLINE AbortImpl(const char *file, int line, const char *func, const char *expr, u64 value);

    void NORETURN NOINLINE AbortImpl(const AbortCtx&);
};