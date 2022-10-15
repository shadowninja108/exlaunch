#include "jit.hpp"

namespace exl::util::jit {

    namespace {
        __attribute__((section (".text.jit_area")))
        alignas(PAGE_SIZE) static const u8 s_JitArea[setting::JitSize] {};
    }
    
    util::TypedStorage<RwPages> impl::s_Pages;
    
    void Initialize() {
        util::ConstructAt(impl::s_Pages, reinterpret_cast<uintptr_t>(&s_JitArea), sizeof(s_JitArea));
    }

    void Flush() {
        const auto& claim = GetPages().GetClaim();
        armDCacheFlush((void*)claim.GetAlignedRw(), claim.GetAlignedSize());
        armICacheInvalidate((void*)claim.GetAlignedRw(), claim.GetAlignedSize());
    }

}