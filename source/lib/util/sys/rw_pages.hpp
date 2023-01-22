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

                constexpr uintptr_t GetAlignedRo() const {
                    return ALIGN_DOWN(m_Ro, PAGE_SIZE);
                }

                constexpr uintptr_t GetAlignedRw() const {
                    return ALIGN_DOWN(m_Rw, PAGE_SIZE);
                }

                constexpr size_t GetAlignedSize() const {
                    return ALIGN_UP(m_Size, PAGE_SIZE);
                }

                constexpr ptrdiff_t RoToOffset(uintptr_t address) const {
                    return m_Ro - address;
                }

                constexpr ptrdiff_t RwToOffset(uintptr_t address) const {
                    return m_Rw - address;
                }

                constexpr uintptr_t RoToRw(uintptr_t address) const {
                    return m_Rw + RoToOffset(address);
                }

                constexpr uintptr_t RwToRo(uintptr_t address) const {
                    return RwToOffset(address) + m_Ro;
                }

                constexpr bool InRo(uintptr_t address) const {
                    ptrdiff_t offset = RoToOffset(address);

                    /* Is the address before the ro region? */
                    if(offset < 0)
                        return false;

                    /* Is the address after the ro region? */
                    if(m_Size <= static_cast<uintptr_t>(offset))
                        return false;

                    return true;
                }

                constexpr bool InRw(uintptr_t address) const {
                    ptrdiff_t offset = RwToOffset(address);

                    /* Is the address before the rw region? */
                    if(offset < 0)
                        return false;

                    /* Is the address after the rw region? */
                    if(m_Size <= static_cast<uintptr_t>(offset))
                        return false;

                    return true;
                }
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

            void Flush();

            inline const Claim& GetClaim() const { return m_Claim; }

            inline uintptr_t GetRo() const { return GetClaim().m_Ro; }
            inline uintptr_t GetRw() const { return GetClaim().m_Rw; }
            inline uintptr_t GetSize() const{ return GetClaim().m_Size; }

            ~RwPages();        
        };
};