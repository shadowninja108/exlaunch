#include "rw_pages.hpp"

#include "lib/util/cur_proc_handle.hpp"
#include <cstring>
#include <algorithm>

namespace exl::util {

    RwPages::RwPages(uintptr_t ro, size_t size)  {
        /* Setup variables. */
        uintptr_t alignedRo = ALIGN_DOWN(ro, PAGE_SIZE);
        size_t alignedSize = ALIGN_UP(size, PAGE_SIZE);

        /* Reserve space to map for RW. */
        uintptr_t baseRw = (uintptr_t) virtmemFindAslr(alignedSize, 0);
        EXL_ASSERT(baseRw != 0);
        auto reserve = virtmemAddReservation((void*)baseRw, alignedSize);
        EXL_ASSERT(reserve != NULL);

        auto procHandle = proc_handle::Get();

        /* Iterate through every range and unmap. */
        MemoryInfo meminfo {
            .addr = alignedRo
        };
        uintptr_t offset = 0;
        u32 pageinfo;
        do {
            /* Query next range. */
            R_ABORT_UNLESS(svcQueryMemory(&meminfo, &pageinfo, meminfo.addr + meminfo.size));

            /* Calculate correct pointers for each mapping. */
            u64 cro = alignedRo + offset;
            u64 sizeLeft = alignedSize - offset;
            u64 offsetInCurRange = cro - meminfo.addr;
            auto size = std::min(sizeLeft, meminfo.size - offsetInCurRange);
            void* crw = (void*) (baseRw + offset);

            /* Map. */
            R_ABORT_UNLESS(svcMapProcessMemory(crw, procHandle, cro, size));

            offset += size;
            
        /* Exit once we've mapped enough pages. */
        } while(offset < alignedSize);

        /* Setup RW pointer to match same physical location of RX. */
        auto rw = baseRw + (ro - alignedRo);

        /* Ensure the data at the different mapping is the same. */
        EXL_ASSERT(memcmp((void*)ro, (void*)rw, size) == 0);

        m_Claim = { 
            .m_Ro = ro,
            .m_Rw = rw,
            .m_Size = size,
            .m_RwReserve = reserve
        };
    }

    RwPages::~RwPages() {
        /* Only unclaim if this is the owner. */
        if(!m_Owner) return;

        auto& claim = m_Claim;

        /* Setup variables. */
        uintptr_t alignedRo = ALIGN_DOWN(claim.m_Ro, PAGE_SIZE);
        uintptr_t alignedRw = ALIGN_DOWN(claim.m_Rw, PAGE_SIZE);
        size_t alignedSize   = ALIGN_UP(claim.m_Size, PAGE_SIZE);

        /* Flush data. */
        armDCacheFlush((void*)claim.m_Rw, claim.m_Size);
        armICacheInvalidate((void*)claim.m_Ro, claim.m_Size);

        auto procHandle = proc_handle::Get();

        /* Iterate through every range and unmap. */
        MemoryInfo meminfo {
            .addr = alignedRo
        };
        uintptr_t offset = 0;
        u32 pageinfo;
        do {
            /* Query next range. */
            R_ABORT_UNLESS(svcQueryMemory(&meminfo, &pageinfo, meminfo.addr + meminfo.size));

            /* Calculate correct pointers for each mapping. */
            u64 cro = alignedRo + offset;
            u64 sizeLeft = alignedSize - offset;
            u64 offsetInCurRange = cro - meminfo.addr;
            auto size = std::min(sizeLeft, meminfo.size - offsetInCurRange);
            void* crw = (void*) (alignedRw + offset);

            /* Unmap. */
            R_ABORT_UNLESS(svcUnmapProcessMemory(crw, procHandle, cro, size));
            
            offset += size;

        /* Exit once we've unmapped enough pages. */
        } while(offset < alignedSize);

        /* Free RW reservation. */
        virtmemRemoveReservation(claim.m_RwReserve);
    }
};
