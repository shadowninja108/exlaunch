#pragma once

#include "common.hpp"

namespace exl::util {
    
    class RwPages {
    private:
        bool m_IsClaimed;
        size_t m_Size;

    public:
        uintptr_t m_Rx;
        uintptr_t m_Rw;

        RwPages(uintptr_t addr, size_t size);

        void Claim();
        void Unclaim();
        
        ~RwPages();
    };
};