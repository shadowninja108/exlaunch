#pragma once

#include "nn_common.hpp"

extern "C" int _nnmusl_errno_from_result(Result);
extern "C" int __nnmusl_BroadcastConditionVariable(void*);