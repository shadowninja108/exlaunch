/**
 * @file smc.h
 * @brief Wrappers for secure monitor calls.
 * @copyright libnx Authors
 */
#pragma once
#include "types.h"
#include "kernel/svc.h"

typedef enum {
    SplConfigItem_DisableProgramVerification = 1,
    SplConfigItem_DramId = 2,
    SplConfigItem_SecurityEngineIrqNumber = 3,
    SplConfigItem_Version = 4,
    SplConfigItem_HardwareType = 5,
    SplConfigItem_IsRetail = 6,
    SplConfigItem_IsRecoveryBoot = 7,
    SplConfigItem_DeviceId = 8,
    SplConfigItem_BootReason = 9,
    SplConfigItem_MemoryArrange = 10,
    SplConfigItem_IsDebugMode = 11,
    SplConfigItem_KernelMemoryConfiguration = 12,
    SplConfigItem_IsChargerHiZModeEnabled = 13,
    SplConfigItem_IsKiosk = 14,
    SplConfigItem_NewHardwareType = 15,
    SplConfigItem_NewKeyGeneration = 16,
    SplConfigItem_Package2Hash = 17,

    SplConfigItem_ExosphereVersion = 65000,
    SplConfigItem_NeedsReboot      = 65001,
    SplConfigItem_NeedsShutdown    = 65002,
    SplConfigItem_ExosphereVerHash = 65003,
    SplConfigItem_HasRcmBugPatch   = 65004,
} SplConfigItem;

typedef enum {
    SplHardwareType_Icosa  = 0,
    SplHardwareType_Copper = 1,
    SplHardwareType_Hoag   = 2,
    SplHardwareType_Iowa   = 3,
    SplHardwareType_Calcio = 4,
    SplHardwareType_Aula   = 5,
} SplHardwareType;

Result smcGetConfig(SplConfigItem config_item, u64 *out_config);

void smcRebootToRcm(void);
void smcRebootToIramPayload(void);
void smcPerformShutdown(void);

Result smcCopyToIram(uintptr_t iram_addr, const void *src_addr, u32 size);
Result smcCopyFromIram(void *dst_addr, uintptr_t iram_addr, u32 size);

Result smcReadWriteRegister(u32 phys_addr, u32 value, u32 mask);

Result smcGenerateRandomBytes(void *dst, u32 size);
Result smcGenerateRandomU64(u64* out);