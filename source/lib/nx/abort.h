// Provide a C shim for aborting in libnx
#pragma once

#include "types.h"

extern void exl_abort(Result);

#ifndef R_ABORT_UNLESS
#define R_ABORT_UNLESS(r)   \
    {                       \
    Result _tmp_r = r;      \
    if(R_FAILED(_tmp_r))    \
        exl_abort(_tmp_r);   \
    }                                       

#elif
#error "Included abort.h outside of libnx!"
#endif