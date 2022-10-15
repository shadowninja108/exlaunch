#pragma once

#include "common.hpp"
#include "lib/util/typed_storage.hpp"
#include "rw_pages.hpp"

namespace exl::util::jit {

    namespace impl {
        extern util::TypedStorage<RwPages> s_Pages;
    }

    void Initialize();
    void Flush();

    inline const RwPages& GetPages() { return util::GetReference(impl::s_Pages); }
    inline uintptr_t GetRo() { return GetPages().GetRo(); }
    inline uintptr_t GetRw() { return GetPages().GetRw(); }
    inline uintptr_t GetSize() { return GetPages().GetSize(); }
}