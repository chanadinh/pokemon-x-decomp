108aa67c  f8402de9  stmdb sp!,{r3,r4,r5,r6,r7,lr}
108aa680  0040a0e1  cpy r4, r0
108aa684  0170a0e1  cpy r7, r1
108aa688  0250a0e1  cpy r5, r2
108aa68c  0200a0e3  mov r0, #0x2
108aa690  a104ffeb  bl 0x1086b91c
108aa694  050050e1  cmp r0, r5
108aa698  0e00001a  bne 0x108aa6d8
108aa69c  0510a0e1  cpy r1, r5
108aa6a0  0700a0e1  cpy r0, r7
108aa6a4  7da7feeb  bl 0x108544a0
108aa6a8  0400a0e1  cpy r0, r4
108aa6ac  0360a0e3  mov r6, #0x3
108aa6b0  b803d0e1  ldrh r0, [r0,#0x38]
108aa6b4  0610a0e1  cpy r1, r6
108aa6b8  2d8afeeb  bl 0x1084cf74
108aa6bc  0010a0e3  mov r1, #0x0
108aa6c0  0030a0e1  cpy r3, r0
108aa6c4  00108de5  str r1, [sp,#0x0]
108aa6c8  0520a0e1  cpy r2, r5
108aa6cc  0710a0e1  cpy r1, r7
108aa6d0  0400a0e1  cpy r0, r4
108aa6d4  af86feeb  bl 0x1084c198
108aa6d8  f880bde8  ldmia sp!,{r3,r4,r5,r6,r7,pc}
