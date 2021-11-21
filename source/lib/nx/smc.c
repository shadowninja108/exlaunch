/**
 * @file smc.c
 * @copyright libnx Authors
 */

#include <stddef.h>
#include <string.h>
#include "smc.h"

void smcRebootToRcm(void)
{
    SecmonArgs args;
    args.X[0] = 0xC3000401;                /* smcSetConfig */
    args.X[1] = SplConfigItem_NeedsReboot; /* Exosphere reboot */
    args.X[3] = 1;                         /* Perform reboot to RCM. */
    svcCallSecureMonitor(&args);
}

void smcRebootToIramPayload(void)
{
    SecmonArgs args;
    args.X[0] = 0xC3000401;                /* smcSetConfig */
    args.X[1] = SplConfigItem_NeedsReboot; /* Exosphere reboot */
    args.X[3] = 2;                         /* Perform reboot to payload at 0x40010000 in IRAM. */
    svcCallSecureMonitor(&args);
}

void smcPerformShutdown(void)
{
    SecmonArgs args;
    args.X[0] = 0xC3000401;                  /* smcSetConfig */
    args.X[1] = SplConfigItem_NeedsShutdown; /* Exosphere shutdown */
    args.X[3] = 1;                           /* Perform shutdown. */
    svcCallSecureMonitor(&args);
}

Result smcGetConfig(SplConfigItem config_item, u64 *out_config)
{
    SecmonArgs args;
    args.X[0] = 0xC3000002;       /* smcGetConfig */
    args.X[1] = (u64)config_item; /* config item */
    Result rc = svcCallSecureMonitor(&args);
    if (rc == 0)
    {
        if (args.X[0] == 0)
        {
            if (out_config)
            {
                *out_config = args.X[1];
            }
        }
        else
        {
            /* SPL result n = SMC result n */
            rc = (26u | ((u32)args.X[0] << 9u));
        }
    }
    return rc;
}

Result smcCopyToIram(uintptr_t iram_addr, const void *src_addr, u32 size)
{
    SecmonArgs args;
    args.X[0] = 0xF0000201;     /* smcAmsIramCopy */
    args.X[1] = (u64)src_addr;  /* DRAM address */
    args.X[2] = (u64)iram_addr; /* IRAM address */
    args.X[3] = size;           /* Amount to copy */
    args.X[4] = 1;              /* 1 = Write */
    Result rc = svcCallSecureMonitor(&args);
    if (rc == 0)
    {
        if (args.X[0] != 0)
        {
            /* SPL result n = SMC result n */
            rc = (26u | ((u32)args.X[0] << 9u));
        }
    }
    return rc;
}

Result smcCopyFromIram(void *dst_addr, uintptr_t iram_addr, u32 size)
{
    SecmonArgs args;
    args.X[0] = 0xF0000201;     /* smcAmsIramCopy */
    args.X[1] = (u64)dst_addr;  /* DRAM address */
    args.X[2] = (u64)iram_addr; /* IRAM address */
    args.X[3] = size;           /* Amount to copy */
    args.X[4] = 0;              /* 0 = Read */
    Result rc = svcCallSecureMonitor(&args);
    if (rc == 0)
    {
        if (args.X[0] != 0)
        {
            /* SPL result n = SMC result n */
            rc = (26u | ((u32)args.X[0] << 9u));
        }
    }
    return rc;
}

Result smcReadWriteRegister(u32 phys_addr, u32 value, u32 mask)
{
    SecmonArgs args;
    args.X[0] = 0xF0000002; /* smcAmsReadWriteRegister */
    args.X[1] = phys_addr;  /* MMIO address */
    args.X[2] = mask;       /* mask */
    args.X[3] = value;      /* value */
    Result rc = svcCallSecureMonitor(&args);
    if (rc == 0)
    {
        if (args.X[0] != 0)
        {
            /* SPL result n = SMC result n */
            rc = (26u | ((u32)args.X[0] << 9u));
        }
    }
    return rc;
}

Result smcGenerateRandomBytes(void *dst, u32 size)
{
    SecmonArgs args;
    args.X[0] = 0xC3000006;     /* smcGenerateRandomBytes */
    args.X[1] = size;
    Result rc = svcCallSecureMonitor(&args);
    if (rc == 0)
    {
        if (args.X[0] != 0)
        {
            /* SPL result n = SMC result n */
            rc = (26u | ((u32)args.X[0] << 9u));
        }
        if (rc == 0)
        {
            memcpy(dst, &args.X[1], size);
        }
    }
    return rc;
}

Result smcGenerateRandomU64(u64* out)
{
    u64 random;

    R_TRY(smcGenerateRandomBytes(&random, sizeof(random)));
    *out = random;
    
    return 0;
}