// Copyright 2017 plutoo
#include "../types.h"
#include "detect.h"
//#include "mutex.h"
#include "svc.h"
#include <malloc.h>

static bool g_VersionCached;
// static Mutex g_VersionMutex;
static int g_Version;

// static bool g_JitKernelPatchCached;
// static Mutex g_JitKernelPatchMutex;
// static bool g_JitKernelPatchDetected;

static void _CacheVersion(void)
{
    if (__atomic_load_n(&g_VersionCached, __ATOMIC_SEQ_CST))
        return;

    /* TODO: thread safety */

    if (g_VersionCached) {
        return;
    }

    u64 tmp;
    g_Version = 1;
    if (R_VALUE(svcGetInfo(&tmp, InfoType_AslrRegionAddress, INVALID_HANDLE, 0)) != KERNELRESULT(InvalidEnumValue))
        g_Version = 2;
    if (R_VALUE(svcGetInfo(&tmp, InfoType_ProgramId, INVALID_HANDLE, 0)) != KERNELRESULT(InvalidEnumValue))
        g_Version = 3;
    if (R_VALUE(svcGetInfo(&tmp, InfoType_InitialProcessIdRange, INVALID_HANDLE, 0)) != KERNELRESULT(InvalidEnumValue))
        g_Version = 4;
    if (R_VALUE(svcGetInfo(&tmp, InfoType_UserExceptionContextAddress, INVALID_HANDLE, 0)) != KERNELRESULT(InvalidEnumValue))
        g_Version = 5;
    if (R_VALUE(svcGetInfo(&tmp, InfoType_TotalNonSystemMemorySize, INVALID_HANDLE, 0)) != KERNELRESULT(InvalidEnumValue))
        g_Version = 6;

    __atomic_store_n(&g_VersionCached, true, __ATOMIC_SEQ_CST);

}
int detectKernelVersion(void) {
    _CacheVersion();
    return g_Version;
}