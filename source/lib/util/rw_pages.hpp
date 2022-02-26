#pragma once

#include <common.hpp>
#include <utility>
#include <new>

namespace exl::util {
    
    class RwPages {
        NON_COPYABLE(RwPages);
        private:
            struct Claim {
                uintptr_t m_Ro = 0;
                uintptr_t m_Rw = 0;
                size_t m_Size = 0;
                VirtmemReservation* m_RwReserve = nullptr;
            };

            Claim m_Claim;
            bool m_Owner = true;

        public:
            RwPages(uintptr_t ro, size_t size);
            
            /* Explicitly only allow moving. */
            RwPages(RwPages&& other) 
            : m_Claim(std::exchange(other.m_Claim, {})), 
            m_Owner(std::exchange(other.m_Owner, false)) {}
            RwPages& operator=(RwPages&& other) {
                m_Claim = std::exchange(other.m_Claim, {});
                other.m_Owner = false;
                return *this;
            }

            inline uintptr_t GetRo() const { return m_Claim.m_Ro; }
            inline uintptr_t GetRw() const { return m_Claim.m_Rw; }

            ~RwPages();        
        };
};