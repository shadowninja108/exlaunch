#pragma once

#include "types.h"
#include "lib/diag/assert.hpp"

#define R_ABORT_UNLESS(expr)    \
    {                           \
        Result _tmp_r = expr;   \
        if(R_FAILED(_tmp_r))    \
            EXL_ABORT(_tmp_r);  \
    }

namespace exl::result {
    using BaseType = u32;
    constexpr BaseType SuccessValue = BaseType();
    constexpr Result MakeResult(BaseType m, BaseType d) {
        return MAKERESULT(m, d);
    }

    constexpr BaseType ExlModule = 252;

    constexpr Result Success                        =  MakeResult(0, SuccessValue); 

    constexpr Result HookFailed                     = MakeResult(ExlModule, 1);
    constexpr Result HookTrampolineAllocFail        = MakeResult(ExlModule, 2);
    constexpr Result HookFixingTooManyInstructions  = MakeResult(ExlModule, 3);
    constexpr Result FailedToFindTarget             = MakeResult(ExlModule, 4);
    constexpr Result TooManyStaticModules           = MakeResult(ExlModule, 5);
    
}