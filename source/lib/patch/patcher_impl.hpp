#pragma once

#include <lib/util/sys/rw_pages.hpp>
#include <lib/util/sys/mem_layout.hpp>
#include <lib/util/typed_storage.hpp>
#include <optional>

namespace exl::patch {

    namespace impl {
        inline util::TypedStorage<const util::RwPages> s_Storage;

        inline const util::RwPages& GetRwPages() { return util::GetReference(s_Storage); }

        inline void InitPatcherImpl() {
            auto& mod = util::GetMainModuleInfo();
            auto start = mod.m_Total.m_Start;
            auto size = mod.m_Rodata.GetEnd() - start;
            util::ConstructAt(s_Storage, start, size);
        }
    };

    class PatcherImpl {
        protected:
        const util::RwPages& m_Pages;

        template<typename T>
        inline T& At(const uintptr_t offset) {
            /* Get address for the object. */
            uintptr_t start = RwFromAddr(offset);

            auto ptr = reinterpret_cast<T*>(start);
            return *ptr;
        }

        public:
        inline PatcherImpl() : m_Pages(impl::GetRwPages()) {}

        inline uintptr_t AddrFromRo(uintptr_t ro) const { return ro - m_Pages.GetRo(); }
        inline uintptr_t AddrFromRw(uintptr_t rw) const { return rw - m_Pages.GetRw(); }

        inline uintptr_t RoFromAddr(uintptr_t addr) const { return m_Pages.GetRo() + addr; }
        inline uintptr_t RwFromAddr(uintptr_t addr) const { return m_Pages.GetRw() + addr; }

        inline ptrdiff_t AddrFromRoPointer(void* ptr) const { return AddrFromRo(reinterpret_cast<uintptr_t>(ptr)); }
        inline ptrdiff_t AddrFromRwPointer(void* ptr) const { return AddrFromRw(reinterpret_cast<uintptr_t>(ptr)); }
    };
}