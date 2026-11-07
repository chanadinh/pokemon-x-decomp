10867bbc  02c0a0e1  cpy r12, r2
10867bc0  04e02de5  str lr, [sp,#-0x4]!
10867bc4  cc209fe5  ldr r2, [0x10867c98]
10867bc8  0c0090e5  ldr r0, [r0,#0xc]
10867bcc  01005ce3  cmp r12, #0x1
10867bd0  14d04de2  sub sp, sp, #0x14
10867bd4  0b00000a  beq 0x10867c08
10867bd8  02005ce3  cmp r12, #0x2
10867bdc  1000000a  beq 0x10867c24
10867be0  03005ce3  cmp r12, #0x3
10867be4  1700000a  beq 0x10867c48
10867be8  04005ce3  cmp r12, #0x4
10867bec  1f00000a  beq 0x10867c70
10867bf0  0230a0e1  cpy r3, r2
10867bf4  14d08de2  add sp, sp, #0x14
10867bf8  0120a0e1  cpy r2, r1
10867bfc  04e09de4  ldr lr, [sp],#0x4
10867c00  6910a0e3  mov r1, #0x69
10867c04  750e00ea  b 0x1086b5e0
10867c08  00208de5  str r2, [sp,#0x0]
10867c0c  0120a0e1  cpy r2, r1
10867c10  003093e5  ldr r3, [r3,#0x0]
10867c14  6910a0e3  mov r1, #0x69
10867c18  700e00eb  bl 0x1086b5e0
10867c1c  14d08de2  add sp, sp, #0x14
10867c20  04f09de4  ldr pc, [sp],#0x4
10867c24  04c093e5  ldr r12, [r3,#0x4]
10867c28  04208de5  str r2, [sp,#0x4]
10867c2c  0120a0e1  cpy r2, r1
10867c30  00c08de5  str r12, [sp,#0x0]
10867c34  003093e5  ldr r3, [r3,#0x0]
10867c38  6910a0e3  mov r1, #0x69
10867c3c  670e00eb  bl 0x1086b5e0
10867c40  14d08de2  add sp, sp, #0x14
10867c44  04f09de4  ldr pc, [sp],#0x4
10867c48  04c083e2  add r12, r3, #0x4
10867c4c  020a9cec  vldmia r12, {s0,s1}
10867c50  020a8dec  vstmia sp, {s0,s1}
10867c54  08208de5  str r2, [sp,#0x8]
10867c58  0120a0e1  cpy r2, r1
10867c5c  003093e5  ldr r3, [r3,#0x0]
10867c60  6910a0e3  mov r1, #0x69
10867c64  5d0e00eb  bl 0x1086b5e0
10867c68  14d08de2  add sp, sp, #0x14
10867c6c  04f09de4  ldr pc, [sp],#0x4
10867c70  04c083e2  add r12, r3, #0x4
10867c74  030a9cec  vldmia r12, {s0,s1,s2}
10867c78  030a8dec  vstmia sp, {s0,s1,s2}
10867c7c  0c208de5  str r2, [sp,#0xc]
10867c80  0120a0e1  cpy r2, r1
10867c84  003093e5  ldr r3, [r3,#0x0]
10867c88  6910a0e3  mov r1, #0x69
10867c8c  530e00eb  bl 0x1086b5e0
10867c90  14d08de2  add sp, sp, #0x14
10867c94  04f09de4  ldr pc, [sp],#0x4
