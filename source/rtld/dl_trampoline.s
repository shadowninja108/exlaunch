.section ".text", "ax"

#define ip0 x16
#define ip1 x17

.global __rtld_runtime_resolve
.type __rtld_runtime_resolve, @function
__rtld_runtime_resolve:
    /* AArch64 we get called with:
    ip0 (x16)         &PLTGOT[2]
    ip1 (x17)         temp(dl resolver entry point)
    [sp, #0]          &PLTGOT[n]

    What we need:
    x0 = calling module (ip0[-1])
    x1 = .rel.plt index ((ip1 - ip0 - 8) / 8)
    */
    ldr x17, [sp]
    str x29, [sp]
    stp x8, x19, [sp, #-0x10]!
    stp x6, x7, [sp, #-0x10]!
    stp x4, x5, [sp, #-0x10]!
    stp x2, x3, [sp, #-0x10]!
    stp x0, x1, [sp, #-0x10]!
    stp q6, q7, [sp, #-0x20]!
    stp q4, q5, [sp, #-0x20]!
    stp q2, q3, [sp, #-0x20]!
    stp q0, q1, [sp, #-0x20]!
    mov x29, sp
    mov x19, x17
    sub x1, x17, x16
    sub x1, x1, #8
    lsr x1, x1, #3
    ldur x0, [x16, #-8]
    bl __rtld_lazy_bind_symbol
    str x0, [x19]
    mov x16, x0
    ldp q0, q1, [sp], #0x20
    ldp q2, q3, [sp], #0x20
    ldp q4, q5, [sp], #0x20
    ldp q6, q7, [sp], #0x20
    ldp x0, x1, [sp], #0x10
    ldp x2, x3, [sp], #0x10
    ldp x4, x5, [sp], #0x10
    ldp x6, x7, [sp], #0x10
    ldp x8, x19, [sp], #0x10
    ldp x29, x30, [sp], #0x10
    br x16