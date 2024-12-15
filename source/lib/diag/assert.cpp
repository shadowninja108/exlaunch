#include "common.hpp"

#include "assert.hpp"
#include "abort.hpp"
#include "log_types.hpp"

#include <program/setting.hpp>
#include <lib/log/logger_mgr.hpp>
#include <program/loggers.hpp>
#include <cstdarg>
#include <cinttypes>

namespace exl::diag {

    namespace {

        inline NORETURN void AbortWithCtx(const AbortCtx & ctx) {
            #ifdef EXL_SUPPORTS_REBOOTPAYLOAD
            /* Ensure abort handler doesn't recursively abort. */
            static std::atomic<bool> recurse_guard;
            auto recursing = recurse_guard.exchange(true);

            if(!recursing && util::IsSocErista()) {
                /* Reboot to abort payload.*/
                AbortToPayload(ctx);
            } else
            #endif
            {
                /* We have no capability of chainloading payloads on mariko. */
                /* Don't have a great solution for this at the moment, just data abort. */
                /* TODO: maybe write to a file? custom fatal program? */

                /* Have lower 32-bits of the address be the lower 32-bits of the value. This ensures users can see (most of) the value even only given the bad address. */
                register u64 addr __asm__("x27") = (0x69696969ull << 32) | static_cast<u32>(ctx.m_Value);
                register u64 val __asm__("x28")  = ctx.m_Value;
                while (true) {
                    __asm__ __volatile__ (
                        "str %[val], [%[addr]]"
                        :
                        : [val]"r"(val), [addr]"r"(addr)
                    );
                }
            }

            UNREACHABLE;
        }

        AbortReason ToAbortReason(AssertionType type) {
            switch (type) {
                case AssertionType_Audit:  return AbortReason_Audit;
                case AssertionType_Assert: return AbortReason_Assert;
                default:
                    return AbortReason_Abort;
            }
        }

        void InvokeAssertionFailureHandler(const AssertionInfo &info) {
            /* TODO: support continuing from assertion. */
            const AbortInfo abort_info = {
                ToAbortReason(info.type),
                info.message,
                info.expr,
                info.func,
                info.file,
                info.line,
            };

            AbortWithCtx({ 0, abort_info });
        }
    }


    void AbortImpl(const char *expr, const char *func, const char *file, int line) {
        #if defined(EXL_DEBUG)
        {
            Logging.Log("Abort Called");
            Logging.Log("    Location:   %s:%d", file, line);
            Logging.Log("    Function:   %s", func);
            Logging.Log("    Expression: %s", expr);
            Logging.Log("");
        }
        #else
        EXL_UNUSED(file, line, func, expr);
        #endif

        /* Create abort info. */
        std::va_list vl{};
        const diag::LogMessage message = { "", std::addressof(vl) };

        const AbortInfo abort_info = {
            AbortReason_Abort,
            std::addressof(message),
            expr,
            func,
            file,
            line,
        };

        AbortWithCtx({ 0, abort_info });
    }

    void AbortImpl(const char *expr, const char *func, const char *file, int line, const char *format, ...) {
        #if defined(EXL_DEBUG)
        {
            Logging.Log("Abort Called");
            Logging.Log("    Location:   %s:%d", file, line);
            Logging.Log("    Function:   %s", func);
            Logging.Log("    Expression: %s", expr);
            Logging.Log("");
            {
                ::std::va_list vl;
                va_start(vl, format);
                Logging.VLog(format, vl);
                va_end(vl);
                Logging.Log("");
            }
        }
        #else
        EXL_UNUSED(file, line, func, expr, format);
        #endif

        /* Create abort info. */
        std::va_list vl;
        va_start(vl, format);
        const diag::LogMessage message = { format, std::addressof(vl) };

        const AbortInfo abort_info = {
            AbortReason_Abort,
            std::addressof(message),
            expr,
            func,
            file,
            line,
        };

        AbortWithCtx({ 0, abort_info });
    }

    void AbortImpl(const char *expr, const char *func, const char *file, int line, const ::exl::Result *result, const char *format, ...) {
        #if defined(EXL_DEBUG)
        {
            Logging.Log("Abort Called");
            Logging.Log("    Location:   %s:%d", file, line);
            Logging.Log("    Function:   %s", func);
            Logging.Log("    Expression: %s", expr);
            Logging.Log("    Result:     0x%08" PRIx32 "", result);
            Logging.Log("");
            {
                ::std::va_list vl;
                va_start(vl, format);
                Logging.VLog(format, vl);
                va_end(vl);
                Logging.Log("");
            }
        }
        #else
        EXL_UNUSED(file, line, func, expr, result, format);
        #endif

        /* Create abort info. */
        std::va_list vl;
        va_start(vl, format);
        const diag::LogMessage message = { format, std::addressof(vl) };

        const AbortInfo abort_info = {
            AbortReason_Abort,
            std::addressof(message),
            expr,
            func,
            file,
            line,
        };

        AbortWithCtx({ *result, abort_info });
    }

    NOINLINE void OnAssertionFailure(AssertionType type, const char *expr, const char *func, const char *file, int line, const char *format, ...) {
        #if defined(EXL_DEBUG)
        {
            Logging.Log("Assertion Failure");
            Logging.Log("    Location:   %s:%d", file, line);
            Logging.Log("    Function:   %s", func);
            Logging.Log("    Expression: %s", expr);
            Logging.Log("");
            {
                ::std::va_list vl;
                va_start(vl, format);
                Logging.VLog(format, vl);
                va_end(vl);
                Logging.Log("");
            }

            EXL_UNUSED(type);
        }
        #else
        EXL_UNUSED(type, expr, func, file, line, format);
        #endif

        /* Create the assertion info. */
        std::va_list vl;
        va_start(vl, format);

        const ::exl::diag::LogMessage message = { format, std::addressof(vl) };

        const AssertionInfo info = {
            type,
            std::addressof(message),
            expr,
            func,
            file,
            line,
        };

        InvokeAssertionFailureHandler(info);
        va_end(vl);
    }


    void OnAssertionFailure(AssertionType type, const char *expr, const char *func, const char *file, int line) {
        return OnAssertionFailure(type, expr, func, file, line, "");
    }
    
    extern "C" void exl_abort(Result result) {
        R_ABORT_UNLESS(result);
    }
}