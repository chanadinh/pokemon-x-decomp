108ae9c8  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108ae9cc  0250a0e1  cpy r5, r2
108ae9d0  0170a0e1  cpy r7, r1
108ae9d4  0300a0e3  mov r0, #0x3
108ae9d8  cff3feeb  bl 0x1086b91c
108ae9dc  050050e1  cmp r0, r5
108ae9e0  1c00001a  bne 0x108aea58
108ae9e4  0600a0e3  mov r0, #0x6
108ae9e8  cbf3feeb  bl 0x1086b91c
108ae9ec  ff6000e2  and r6, r0, #0xff
108ae9f0  0520a0e1  cpy r2, r5
108ae9f4  0b10a0e3  mov r1, #0xb
108ae9f8  0700a0e1  cpy r0, r7
108ae9fc  5eedfeeb  bl 0x10869f7c
108aea00  0040a0e1  cpy r4, r0
108aea04  1d00a0e3  mov r0, #0x1d
108aea08  0f50c4e5  strb r5, [r4,#0xf]
108aea0c  b400c4e1  strh r0, [r4,#0x4]
108aea10  0210a0e3  mov r1, #0x2
108aea14  0600a0e1  cpy r0, r6
108aea18  642bffeb  bl 0x108797b0
108aea1c  080084e5  str r0, [r4,#0x8]
108aea20  34209fe5  ldr r2, [0x108aea5c]
108aea24  0210a0e3  mov r1, #0x2
108aea28  140084e2  add r0, r4, #0x14
108aea2c  287afeeb  bl 0x1084d2d4
108aea30  0510a0e1  cpy r1, r5
108aea34  140084e2  add r0, r4, #0x14
108aea38  177afeeb  bl 0x1084d29c
108aea3c  0610a0e1  cpy r1, r6
108aea40  140084e2  add r0, r4, #0x14
108aea44  147afeeb  bl 0x1084d29c
108aea48  0410a0e1  cpy r1, r4
108aea4c  0700a0e1  cpy r0, r7
108aea50  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108aea54  3be4feea  b 0x10867b48
108aea58  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
