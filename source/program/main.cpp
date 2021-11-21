#include "lib.hpp"

MAKE_HOOK_T(void, stubCopyright, (bool),
    /* Call the original implementation of this function, but with a false argument. */
    impl(false);
);

/* Declare function to dynamic link with. */
namespace nn::oe {
    void SetCopyrightVisibility(bool);
};

extern "C" void exl_main(void* x0, void* x1) {
    /* Setup hooking enviroment. */
    envSetOwnProcessHandle(exl::util::proc_handle::Get());
    exl::hook::Initialize();

    INJECT_HOOK_T(nn::oe::SetCopyrightVisibility, stubCopyright);

    /*
    For sysmodules/applets, you have to call the entrypoint when ready
    exl::hook::CallTargetEntrypoint(x0, x1);
    */
}

extern "C" NORETURN void exl_exception_entry() {
    /* TODO: exception handling */
    EXL_ABORT(0x420);
}