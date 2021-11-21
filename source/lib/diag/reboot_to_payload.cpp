/*
 * Copyright (c) Atmosphère-NX
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef EXL_SUPPORTS_REBOOTPAYLOAD
#include "reboot_to_payload.hpp"

#include "program/abort_payload.inc"

#include <cstring>
#include <algorithm>

namespace exl::diag
{
    namespace
    {

        namespace ams
        {
            constexpr size_t FatalErrorMaxStacktrace = 0x20;
            constexpr size_t FatalErrorMaxStackdump = 0x100;

            /* "AFE1" */
            constexpr u32 RebootToFatalMagic = 0x31454641;

            constexpr uintptr_t IramBase = 0x40000000ull;
            constexpr uintptr_t IramPayloadBase = 0x40010000ull;
            constexpr size_t IramSize = 0x40000;
            constexpr size_t IramPayloadMaxSize = 0x2E000;
            constexpr uintptr_t FatalErrorBase = IramPayloadBase + IramPayloadMaxSize;

            struct FatalErrorCtx
            {
                u32 magic;
                u32 error_desc;
                u64 title_id;
                union
                {
                    u64 gprs[32];
                    struct
                    {
                        u64 _gprs[29];
                        u64 fp;
                        u64 lr;
                        u64 sp;
                    };
                };
                u64 pc;
                u64 module_base;
                u32 pstate;
                u32 afsr0;
                u32 afsr1;
                u32 esr;
                u64 far;
                u64 report_identifier; /* Normally just system tick. */
                u64 stack_trace_size;
                u64 stack_dump_size;
                u64 stack_trace[FatalErrorMaxStacktrace];
                uint8_t stack_dump[FatalErrorMaxStackdump];
            };
        };

        alignas(PAGE_SIZE) u8 WorkPage[PAGE_SIZE];
        constexpr std::size_t AbortPayloadSize = sizeof(s_AbortPayload);

        /* Enure provided payload can fit in IRAM space. */
        static_assert(AbortPayloadSize <= ams::IramPayloadMaxSize);

        void ClearIram()
        {
            std::memset(WorkPage, 0xCC, sizeof(WorkPage));

            for (size_t ofs = 0; ofs < ams::IramSize; ofs += sizeof(WorkPage))
            {
                smcCopyToIram(ams::IramBase + ofs, WorkPage, sizeof(WorkPage));
            }
        }

        void CopyAbortPayloadToIram()
        {
            /* Clear IRAM. */
            ClearIram();

            /* Copy payload in sizeof(WorkPage) increments. */
            for (std::size_t ofs = 0; ofs < AbortPayloadSize; ofs += sizeof(WorkPage))
            {
                std::size_t copySize = std::min(AbortPayloadSize - ofs, sizeof(WorkPage));

                std::memcpy(WorkPage, &s_AbortPayload[ofs], copySize);
                std::memset(&WorkPage[copySize], 0xCC, sizeof(WorkPage) - copySize);

                smcCopyToIram(ams::IramPayloadBase + ofs, WorkPage, sizeof(WorkPage));
            }
        }
    };

    void NORETURN AbortToPayload(const AbortCtx& ctx)
    {
        /* Copy abort payload into IRAM. */
        CopyAbortPayloadToIram();

       /* Populate AMS' fatal context. */
        ams::FatalErrorCtx error_ctx = {
            .magic = ams::RebootToFatalMagic,
            .error_desc = ctx.m_Result,
            .title_id = setting::TitleId,
        };

        /* Pad and copy fatal context. */
        std::memset(WorkPage, 0xCC, sizeof(WorkPage));
        std::memcpy(WorkPage, &error_ctx, sizeof(error_ctx));
        smcCopyToIram(ams::FatalErrorBase, WorkPage, sizeof(WorkPage));

        /* Invoke reboot. */
        smcRebootToIramPayload();

        UNREACHABLE;
    }
};

#endif