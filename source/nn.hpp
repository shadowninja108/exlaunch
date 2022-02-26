#pragma once

/* nnSdk is only available when loaded as a module. */
#ifndef EXL_AS_MODULE
#error "Cannot use nnSdk when not as a module!"
#endif

#include "nn/fs.hpp"
#include "nn/os.hpp"
#include "nn/time.hpp"