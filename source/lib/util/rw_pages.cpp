#include "rw_pages.hpp"

#include "lib/util/cur_proc_handle.hpp"
#include <cstring>

namespace exl::util {

    RwPages::RwPages(uintptr_t rx, size_t size) {
        m_Rx = rx;
        m_Size = size;
        m_IsClaimed = false;

        Claim();
    }

    void RwPages::Claim() {
        if(!m_IsClaimed) {
            /* Setup variables. */
            uintptr_t alignedRx = ALIGN_DOWN(m_Rx, PAGE_SIZE);
            size_t alignedSize = ALIGN_UP(m_Size, PAGE_SIZE);

            /* Reserver space to map for RW. */
            uintptr_t baseRw = (uintptr_t) virtmemReserve(alignedSize);

            /* Map RW. */
            R_ABORT_UNLESS(svcMapProcessMemory((void*)baseRw, proc_handle::Get(), alignedRx, alignedSize));

            /* Setup RW pointer to match same physical location of RX. */
            m_Rw = baseRw + (m_Rx - alignedRx);
            m_IsClaimed = true;
        }
    }

    void RwPages::Unclaim() {
        if (m_IsClaimed) {
            /* Setup variables. */
            uintptr_t alignedRx = ALIGN_DOWN(m_Rx, PAGE_SIZE);
            uintptr_t alignedRw = ALIGN_DOWN(m_Rw, PAGE_SIZE);
            size_t alignedSize   = ALIGN_UP(m_Size, PAGE_SIZE);

            /* Flush data. */
            armDCacheFlush((void*)m_Rw, m_Size);
            armICacheInvalidate((void*)m_Rx, m_Size);

            /* Unmap RW. */
            R_ABORT_UNLESS(svcUnmapProcessMemory((void*)alignedRw, proc_handle::Get(), alignedRx, alignedSize));

            /* Free RW. */
            virtmemFree((void*)alignedRw, alignedSize);

            /* Clean up variables. */
            m_Rw = 0;
            m_IsClaimed = false;
        }
    }

    RwPages::~RwPages() {
        Unclaim();
    }
};
