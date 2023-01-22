#pragma once

#include "common.hpp"
#include "lib/util/typed_storage.hpp"
#include "rw_pages.hpp"

#include <span>

#define JIT_CREATE(name, size)                          \
    namespace impl::name {                              \
        __attribute__((section(".text." #name)))        \
        alignas(PAGE_SIZE)                              \
        static const std::array<u8, size> s_Area {};    \
    }                                                   \
    exl::util::Jit name(std::span(impl::name::s_Area));

namespace exl::util {

    class Jit {
        std::span<const u8> m_Rx;
        util::TypedStorage<RwPages> m_Pages;

        inline RwPages& GetPages() { return util::GetReference(m_Pages); }

        public:
        constexpr Jit(std::span<const u8> rx) : m_Rx(rx) {}
        inline void Initialize() {
            util::ConstructAt(m_Pages, reinterpret_cast<uintptr_t>(m_Rx.data()), m_Rx.size());
        }

        inline void Flush() {
            GetPages().Flush();
        }

        inline uintptr_t GetRo() { return GetPages().GetRo(); }
        inline uintptr_t GetRw() { return GetPages().GetRw(); }
        inline uintptr_t GetSize() { return GetPages().GetSize(); }
    };
}