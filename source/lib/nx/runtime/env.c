#include "env.h"

static Handle g_processHandle = INVALID_HANDLE;

Handle envGetOwnProcessHandle(void) {
    return g_processHandle;
}

void envSetOwnProcessHandle(Handle h) {
    g_processHandle = h;
}