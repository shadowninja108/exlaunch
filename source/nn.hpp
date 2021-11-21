#pragma once

/* nnSdk is only available when loaded as a module. */
#if EXL_LOAD_KIND != Module
#error "Cannot use nnSdk when not as a module!"
#endif

#include "nn/fs.hpp"
