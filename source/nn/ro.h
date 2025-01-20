#pragma once

#include <cstdint>

namespace nn::ro {
::Result LookupSymbol(uintptr_t *out, const char *name);
}
