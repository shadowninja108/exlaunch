#include "lib.hpp"
#include <sstream>
#include "nn/fs.hpp"

// MAKE_HOOK_T(void, stubCopyright, (bool),
//     /* Call the original implementation of this function, but with a false argument. */
//     impl(false);
// );

/* Declare function to dynamic link with. */
namespace nn::oe {
    void SetCopyrightVisibility(bool);
};

void hook_callback(exl::util::Hook::ExInlineCtx* ctx) {
    std::stringstream stream;

    for (int i = 0; i < 31; i++) {
        stream << "X" << std::dec << i << ": " << std::hex << ctx->registers[i].x << "\n";
    }

    stream << "SP: " << std::hex << ctx->sp.x << "\n";

    for (int i = 0; i < 32; i++) {
        stream << "V" << std::dec << i << " { " << ctx->v[i].s[0] << ", " << ctx->v[i].s[1] << ", " << ctx->v[i].s[2] << ", " << ctx->v[i].s[3] << " }\n";
    }
    
    auto str = stream.str();
    nn::fs::CreateFile("sd:/reg-dump.txt", 0);
    nn::fs::FileHandle handle;
    R_ABORT_UNLESS(nn::fs::OpenFile(&handle, "sd:/reg-dump.txt", 6));
    R_ABORT_UNLESS(nn::fs::WriteFile(handle, 0, str.c_str(), str.length(), nn::fs::WriteOption::CreateOption(nn::fs::WriteOptionFlag_Flush)));
    nn::fs::CloseFile(handle);
    // EXL_ABORT(0x420);
}

extern "C" void exl_main(void* x0, void* x1) {
    /* Setup hooking enviroment. */
    envSetOwnProcessHandle(exl::util::proc_handle::Get());
    exl::hook::Initialize();

    exl::util::Hook::InlineHook(exl::util::GetMainModuleInfo().m_Text.m_Start + 0x4ec7e0, reinterpret_cast<uintptr_t>(hook_callback), true);

    nn::fs::MountSdCardForDebug("sd");
    /* If an integer is provided instead of a symbol, it will be treated as an offset relative to main. */
    // INJECT_HOOK_T(nn::oe::SetCopyrightVisibility, stubCopyright);

    /*
    For sysmodules/applets, you have to call the entrypoint when ready
    exl::hook::CallTargetEntrypoint(x0, x1);
    */
}

extern "C" NORETURN void exl_exception_entry() {
    /* TODO: exception handling */
    EXL_ABORT(0x420);
}