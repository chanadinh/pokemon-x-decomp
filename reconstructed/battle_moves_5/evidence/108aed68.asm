108aed68  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108aed6c  0060a0e1  cpy r6, r0
108aed70  10d04de2  sub sp, sp, #0x10
108aed74  0170a0e1  cpy r7, r1
108aed78  0250a0e1  cpy r5, r2
108aed7c  0380a0e1  cpy r8, r3
108aed80  0400a0e3  mov r0, #0x4
108aed84  2c74feeb  bl 0x1084be3c
108aed88  0040a0e1  cpy r4, r0
108aed8c  0500a0e1  cpy r0, r5
108aed90  8a74feeb  bl 0x1084bfc0
108aed94  9210a0e3  mov r1, #0x92
108aed98  0c108de5  str r1, [sp,#0xc]
108aed9c  0410a0e3  mov r1, #0x4
108aeda0  13008de8  stmia sp,{r0,r1,r4}
108aeda4  0830a0e1  cpy r3, r8
108aeda8  0520a0e1  cpy r2, r5
108aedac  0710a0e1  cpy r1, r7
108aedb0  0600a0e1  cpy r0, r6
108aedb4  bd73feeb  bl 0x1084bcb0
108aedb8  10d08de2  add sp, sp, #0x10
108aedbc  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
