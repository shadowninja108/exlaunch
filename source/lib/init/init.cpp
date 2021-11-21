#include "common.hpp"

#include "program/setting.hpp"

extern "C" {
    /* These magic symbols are provided by the linker.  */
    extern void (*__preinit_array_start []) (void) __attribute__((weak));
    extern void (*__preinit_array_end []) (void) __attribute__((weak));
    extern void (*__init_array_start []) (void) __attribute__((weak));
    extern void (*__init_array_end []) (void) __attribute__((weak));

    /* Exported by program. */
    extern void exl_main(void*, void*);

    char __fake_heap[exl::setting::HeapSize];

    void __init_heap() {
        extern char * fake_heap_start;
        extern char * fake_heap_end;

        fake_heap_start = __fake_heap;
        fake_heap_end   = __fake_heap + exl::setting::HeapSize;
    }
    
    void __init_array(void) {
        size_t count;
        size_t i;

        count = __preinit_array_end - __preinit_array_start;
        for (i = 0; i < count; i++)
            __preinit_array_start[i] ();

        count = __init_array_end - __init_array_start;
        for (i = 0; i < count; i++)
            __init_array_start[i] ();
    }
    
    /* Called when loaded as a module with RTLD. */
    void exl_module_init() {
        __init_heap();
        virtmemSetup();
        exl_main(NULL, NULL);
    }

    /* Called when loaded as the entrypoint of the process, like RTLD. */
    void exl_entrypoint_init(void* x0, void* x1) {
        __init_heap();
        __init_array();
        virtmemSetup();
        exl_main(x0, x1);
    }

    void exl_module_fini(void) {}
}
