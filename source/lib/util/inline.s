.macro CODE_BEGIN name
    .section .text.\name, "ax", %progbits
    .global \name
    .type \name, %function
    .align 2
    .cfi_startproc
\name:
.endm

.macro CODE_END
    .cfi_endproc
.endm

.macro armBackupRegisters
    sub sp, sp, #0x100
    stp x29, x30, [sp, #0xE8]
    stp x27, x28, [sp, #0xD8]
    stp x25, x26, [sp, #0xC8]
    stp x23, x24, [sp, #0xB8]
    stp x21, x22, [sp, #0xA8]
    stp x19, x20, [sp, #0x98]
    stp x17, x18, [sp, #0x88]
    stp x15, x16, [sp, #0x78]
    stp x13, x14, [sp, #0x68]
    stp x11, x12, [sp, #0x58]
    stp x9, x10, [sp, #0x48]
    stp x7, x8, [sp, #0x38]
    stp x5, x6, [sp, #0x28]
    stp x3, x4, [sp, #0x18]
    stp x1, x2, [sp, #0x8]
    str x0, [sp, #0x0]
.endm

.macro armRecoverRegisters
    ldr x0, [sp, #0x0]
    ldp x1, x2, [sp, #0x8]
    ldp x3, x4, [sp, #0x18]
    ldp x5, x6, [sp, #0x28]
    ldp x7, x8, [sp, #0x38]
    ldp x9, x10, [sp, #0x48]
    ldp x11, x12, [sp, #0x58]
    ldp x13, x14, [sp, #0x68]
    ldp x15, x16, [sp, #0x78]
    ldp x17, x18, [sp, #0x88]
    ldp x19, x20, [sp, #0x98]
    ldp x21, x22, [sp, #0xA8]
    ldp x23, x24, [sp, #0xB8]
    ldp x25, x26, [sp, #0xC8]
    ldp x27, x28, [sp, #0xD8]
    ldp x29, x30, [sp, #0xE8]
    add sp, sp, #0x100
.endm


.macro armBackupExtendedRegisters
    sub sp, sp, #0x300
    
    stp  x0,  x1, [sp,  #0x0]
    stp  x2,  x3, [sp, #0x10]
    stp  x4,  x5, [sp, #0x20]
    stp  x6,  x7, [sp, #0x30]
    stp  x8,  x9, [sp, #0x40]
    stp x10, x11, [sp, #0x40]
    stp x12, x13, [sp, #0x60]
    stp x14, x15, [sp, #0x70]
    stp x16, x17, [sp, #0x80]
    stp x18, x19, [sp, #0x90]
    stp x10, x21, [sp, #0xA0]
    stp x22, x23, [sp, #0xB0]
    stp x24, x25, [sp, #0xC0]
    stp x26, x27, [sp, #0xD0]
    stp x28, x29, [sp, #0xE0]

    add x0, sp, #0x300

    stp x30, x0,  [sp, #0xF0]

    stp  q0,  q1, [sp, #0x100]
    stp  q2,  q3, [sp, #0x120]
    stp  q4,  q5, [sp, #0x140]
    stp  q6,  q7, [sp, #0x160]
    stp  q8,  q9, [sp, #0x180]
    stp q10, q11, [sp, #0x1A0]
    stp q12, q13, [sp, #0x1C0]
    stp q14, q15, [sp, #0x1E0]
    stp q16, q17, [sp, #0x200]
    stp q18, q19, [sp, #0x220]
    stp q20, q21, [sp, #0x240]
    stp q22, q23, [sp, #0x260]
    stp q24, q25, [sp, #0x280]
    stp q26, q27, [sp, #0x2A0]
    stp q28, q29, [sp, #0x2C0]
    stp q30, q31, [sp, #0x2E0]
.endm

.macro armRecoverExtendedRegisters
    ldp  x0,  x1, [sp,  #0x0]
    ldp  x2,  x3, [sp, #0x10]
    ldp  x4,  x5, [sp, #0x20]
    ldp  x6,  x7, [sp, #0x30]
    ldp  x8,  x9, [sp, #0x40]
    ldp x10, x11, [sp, #0x40]
    ldp x12, x13, [sp, #0x60]
    ldp x14, x15, [sp, #0x70]
    ldp x16, x17, [sp, #0x80]
    ldp x18, x19, [sp, #0x90]
    ldp x10, x21, [sp, #0xA0]
    ldp x22, x23, [sp, #0xB0]
    ldp x24, x25, [sp, #0xC0]
    ldp x26, x27, [sp, #0xD0]
    ldp x28, x29, [sp, #0xE0]
    ldr x30, [sp, #0xF0]

    ldp  q0,  q1, [sp, #0x100]
    ldp  q2,  q3, [sp, #0x120]
    ldp  q4,  q5, [sp, #0x140]
    ldp  q6,  q7, [sp, #0x160]
    ldp  q8,  q9, [sp, #0x180]
    ldp q10, q11, [sp, #0x1A0]
    ldp q12, q13, [sp, #0x1C0]
    ldp q14, q15, [sp, #0x1E0]
    ldp q16, q17, [sp, #0x200]
    ldp q18, q19, [sp, #0x220]
    ldp q20, q21, [sp, #0x240]
    ldp q22, q23, [sp, #0x260]
    ldp q24, q25, [sp, #0x280]
    ldp q26, q27, [sp, #0x2A0]
    ldp q28, q29, [sp, #0x2C0]
    ldp q30, q31, [sp, #0x2E0]

    add sp, sp, #0x300
.endm

CODE_BEGIN InlineHandlerImpl
    armBackupRegisters

    mov x0, sp
    ldr x16, [x17, #0x8]
    blr x16

    armRecoverRegisters

    ldr x16, [x17, #0x10]
    br x16
CODE_END

CODE_BEGIN ExInlineHandlerImpl
    armBackupExtendedRegisters

    mov x0, sp
    ldr x16, [x17, #0x8]
    blr x16

    armRecoverExtendedRegisters

    ldr x16, [x17, #0x10]
    br x16
CODE_END

CODE_BEGIN InlineHandler
    adr x17, InlineHandlerEnd
    ldr x16, [x17]
    br x16

CODE_END
InlineHandlerEnd:
    .byte 0x45, 0x58, 0x4C, 0x5F