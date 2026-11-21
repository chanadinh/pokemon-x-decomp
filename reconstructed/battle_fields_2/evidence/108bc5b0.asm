108bc5b0  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108bc5b4  1150a0e3  mov r5, #0x11
108bc5b8  0170a0e1  cpy r7, r1
108bc5bc  90809fe5  ldr r8, [0x108bc654]
108bc5c0  0400a0e3  mov r0, #0x4
108bc5c4  d4bcfeeb  bl 0x1086b91c
108bc5c8  ff6000e2  and r6, r0, #0xff
108bc5cc  0610a0e1  cpy r1, r6
108bc5d0  0700a0e1  cpy r0, r7
108bc5d4  b15ffeeb  bl 0x108544a0
108bc5d8  0510a0e1  cpy r1, r5
108bc5dc  1a41feeb  bl 0x1084ca4c
108bc5e0  000050e3  cmp r0, #0x0
108bc5e4  1900000a  beq 0x108bc650
108bc5e8  0620a0e1  cpy r2, r6
108bc5ec  1810a0e3  mov r1, #0x18
108bc5f0  0700a0e1  cpy r0, r7
108bc5f4  60b6feeb  bl 0x10869f7c
108bc5f8  0450c0e5  strb r5, [r0,#0x4]
108bc5fc  0040a0e1  cpy r4, r0
108bc600  0010a0e1  cpy r1, r0
108bc604  0560c0e5  strb r6, [r0,#0x5]
108bc608  0700a0e1  cpy r0, r7
108bc60c  4dadfeeb  bl 0x10867b48
108bc610  0620a0e1  cpy r2, r6
108bc614  0310a0e3  mov r1, #0x3
108bc618  0700a0e1  cpy r0, r7
108bc61c  56b6feeb  bl 0x10869f7c
108bc620  0050a0e1  cpy r5, r0
108bc624  0820a0e1  cpy r2, r8
108bc628  0210a0e3  mov r1, #0x2
108bc62c  040080e2  add r0, r0, #0x4
108bc630  2743feeb  bl 0x1084d2d4
108bc634  0610a0e1  cpy r1, r6
108bc638  040085e2  add r0, r5, #0x4
108bc63c  1643feeb  bl 0x1084d29c
108bc640  0410a0e1  cpy r1, r4
108bc644  0700a0e1  cpy r0, r7
108bc648  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108bc64c  3dadfeea  b 0x10867b48
108bc650  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
