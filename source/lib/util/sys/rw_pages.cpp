#include "rw_pages.hpp"

#include "lib/util/sys/cur_proc_handle.hpp"
#include <cstring>
#include <algorithm>
#include <functional>

namespace exl::util {

    template<typename Callback>
    static void ForEachMemRange(Callback callback, uintptr_t start_address, size_t length) {
        const uintptr_t end = start_address + length;
        /* Setup variables. */
        MemoryInfo meminfo {
            .addr = start_address
        };
        u32 pageinfo;

        do {
            /* Query next range. */
            R_ABORT_UNLESS(svcQueryMemory(&meminfo, &pageinfo, meminfo.addr + meminfo.size));

            /* Calculate offset into the range we are mapping. */
            uintptr_t offset = std::max(meminfo.addr, start_address) - start_address;
            /* Determine the address we will be working on. */
            uintptr_t current_address = start_address + offset;
            /* Determine the length of the range we will be working on. */
            uintptr_t current_length = std::min(end, meminfo.addr + meminfo.size) - current_address;

            /* Call provided callback. */
            callback(current_address, current_length, offset);

        /* Exit once we've mapped enough pages. */
        } while((meminfo.addr + meminfo.size) < end);
    }

    RwPages::RwPages(uintptr_t ro, size_t size)  {
        /* Initialize the claim with what we know. */
        m_Claim = {
            .m_Ro = ro,
            .m_Size = size,
        };

        /* Get const ref to claim. */
        const auto& claim = GetClaim();

        /* Find space for the corresponding rw region. */
        uintptr_t alignedRw = (uintptr_t) virtmemFindAslr(claim.GetAlignedSize(), 0);
        EXL_ASSERT(alignedRw != 0);

        /* Reserve rw region. */
        auto reserve = virtmemAddReservation((void*)alignedRw, claim.GetAlignedSize());
        EXL_ASSERT(reserve != NULL);
        m_Claim.m_RwReserve = reserve;

        auto procHandle = proc_handle::Get();

        /* Iterate through every range and map. */
        ForEachMemRange(
            [alignedRw, procHandle](uintptr_t address, uintptr_t size, uintptr_t offset) {
                void* rw = (void*) (alignedRw + offset);
                u64 ro = address;

                R_ABORT_UNLESS(svcMapProcessMemory(rw, procHandle, ro, size));
            }
        , claim.GetAlignedRo(), claim.GetAlignedSize());

        /* Setup RW pointer to match same physical location of RX. */
        m_Claim.m_Rw = alignedRw + (ro - claim.GetAlignedRo());

        /* Ensure the data at the different mapping is the same. */
        EXL_ASSERT(memcmp((void*)claim.m_Ro, (void*)claim.m_Rw, size) == 0);
    }

    void RwPages::Flush() {
        const auto& claim = GetClaim();
        armDCacheFlush((void*)claim.GetAlignedRw(), claim.GetAlignedSize());
        armICacheInvalidate((void*)claim.GetAlignedRw(), claim.GetAlignedSize());
    }

    RwPages::~RwPages() {
        /* Only unclaim if this is the owner. */
        if(!m_Owner) return;

        const auto& claim = GetClaim();

        /* Flush data. */
        armDCacheFlush((void*)claim.m_Rw, claim.m_Size);
        armICacheInvalidate((void*)claim.m_Ro, claim.m_Size);

        auto procHandle = proc_handle::Get();
        
        /* Iterate through every range and unmap. */
        ForEachMemRange(
            [&claim, procHandle](uintptr_t address, uintptr_t size, uintptr_t offset) {
                void* rw = reinterpret_cast<void*>(claim.GetAlignedRw() + offset);
                u64 ro = address;

                R_ABORT_UNLESS(svcUnmapProcessMemory(rw, procHandle, ro, size));
            }
        , m_Claim.GetAlignedRo(), m_Claim.GetAlignedSize());

        /* Free RW reservation. */
        virtmemRemoveReservation(claim.m_RwReserve);
    }
};
