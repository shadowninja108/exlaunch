#pragma once

#include "types.h"
#include "lib/diag/assert.hpp"

#include <cinttypes>

#define EXL_CALL_ON_RESULT_ASSERTION_IMPL(cond, val) EXL_CALL_ASSERT_FAIL_IMPL(::exl::diag::AssertionType_Assert, "exl::result", "Failed: %s" PRIX32, cond)
#define EXL_CALL_ON_RESULT_ABORT_IMPL(cond, val)  EXL_CALL_ABORT_IMPL("exl::result", "Failed: %s" PRIX32, cond)

#ifdef EXL_DEBUG
#define R_ASSERT(res_expr)                                                                        \
    {                                                                                             \
        if (const auto _tmp_r_assert_rc = (res_expr); EXL_UNLIKELY(R_FAILED(_tmp_r_assert_rc))) { \
            EXL_CALL_ON_RESULT_ASSERTION_IMPL(#res_expr, _tmp_r_assert_rc);                       \
        }                                                                                         \
    }
#else
#define R_ASSERT(res_expr) EXL_UNUSED((res_expr));
#endif

#define R_ABORT_UNLESS(res_expr)                                                                \
    {                                                                                           \
        if (const auto _tmp_r_abort_rc = (res_expr); EXL_UNLIKELY(R_FAILED(_tmp_r_abort_rc))) { \
            EXL_CALL_ON_RESULT_ABORT_IMPL(#res_expr, _tmp_r_abort_rc);                          \
        }                                                                                       \
    }

namespace exl::result {
    using BaseType = u32;
    constexpr BaseType SuccessValue = BaseType();
    constexpr Result MakeResult(BaseType m, BaseType d) {
        return MAKERESULT(m, d);
    }

    constexpr BaseType ExlModule = 252;

    constexpr Result Success                                    =  MakeResult(0, SuccessValue);

    constexpr Result HookFailed                                 = MakeResult(ExlModule, 1);
    constexpr Result HookTrampolineAllocFail                    = MakeResult(ExlModule, 2);
    constexpr Result HookFixingTooManyInstructions              = MakeResult(ExlModule, 3);
    constexpr Result FailedToFindTarget                         = MakeResult(ExlModule, 4);
    constexpr Result TooManyStaticModules                       = MakeResult(ExlModule, 5);
    constexpr Result VirtualMemberFunctionPointerNotSupported   = MakeResult(ExlModule, 6);
}