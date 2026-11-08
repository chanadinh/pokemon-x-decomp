108bfd64  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108bfd68  0260a0e1  cpy r6, r2
108bfd6c  0170a0e1  cpy r7, r1
108bfd70  0350a0e1  cpy r5, r3
108bfd74  0200a0e3  mov r0, #0x2
108bfd78  e7aefeeb  bl 0x1086b91c
108bfd7c  060050e1  cmp r0, r6
108bfd80  1600001a  bne 0x108bfde0
108bfd84  000095e5  ldr r0, [r5,#0x0]
108bfd88  000050e3  cmp r0, #0x0
108bfd8c  1300000a  beq 0x108bfde0
108bfd90  0620a0e1  cpy r2, r6
108bfd94  0310a0e3  mov r1, #0x3
108bfd98  0700a0e1  cpy r0, r7
108bfd9c  76a8feeb  bl 0x10869f7c
108bfda0  0040a0e1  cpy r4, r0
108bfda4  000090e5  ldr r0, [r0,#0x0]
108bfda8  d220a0e3  mov r2, #0xd2
108bfdac  0210a0e3  mov r1, #0x2
108bfdb0  020580e3  orr r0, r0, #0x800000
108bfdb4  000084e5  str r0, [r4,#0x0]
108bfdb8  040084e2  add r0, r4, #0x4
108bfdbc  4435feeb  bl 0x1084d2d4
108bfdc0  0610a0e1  cpy r1, r6
108bfdc4  040084e2  add r0, r4, #0x4
108bfdc8  3335feeb  bl 0x1084d29c
108bfdcc  0410a0e1  cpy r1, r4
108bfdd0  0700a0e1  cpy r0, r7
108bfdd4  5b9ffeeb  bl 0x10867b48
108bfdd8  0000a0e3  mov r0, #0x0
108bfddc  000085e5  str r0, [r5,#0x0]
108bfde0  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
