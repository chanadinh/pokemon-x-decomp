108c49c8  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108c49cc  0060a0e1  cpy r6, r0
108c49d0  0170a0e1  cpy r7, r1
108c49d4  0240a0e1  cpy r4, r2
108c49d8  0350a0e1  cpy r5, r3
108c49dc  0200a0e3  mov r0, #0x2
108c49e0  cd9bfeeb  bl 0x1086b91c
108c49e4  040050e1  cmp r0, r4
108c49e8  2400001a  bne 0x108c4a80
108c49ec  000095e5  ldr r0, [r5,#0x0]
108c49f0  0680a0e1  cpy r8, r6
108c49f4  0460a0e1  cpy r6, r4
108c49f8  000050e3  cmp r0, #0x0
108c49fc  1f00001a  bne 0x108c4a80
108c4a00  0400a0e1  cpy r0, r4
108c4a04  6d1dfeeb  bl 0x1084bfc0
108c4a08  3c19feeb  bl 0x1084af00
108c4a0c  0090a0e1  cpy r9, r0
108c4a10  0620a0e1  cpy r2, r6
108c4a14  0110a0e3  mov r1, #0x1
108c4a18  0700a0e1  cpy r0, r7
108c4a1c  c91ffeeb  bl 0x1084c948
108c4a20  0620a0e1  cpy r2, r6
108c4a24  0310a0e3  mov r1, #0x3
108c4a28  0700a0e1  cpy r0, r7
108c4a2c  5295feeb  bl 0x10869f7c
108c4a30  0040a0e1  cpy r4, r0
108c4a34  b620a0e3  mov r2, #0xb6
108c4a38  0110a0e3  mov r1, #0x1
108c4a3c  040080e2  add r0, r0, #0x4
108c4a40  2322feeb  bl 0x1084d2d4
108c4a44  0910a0e1  cpy r1, r9
108c4a48  040084e2  add r0, r4, #0x4
108c4a4c  1222feeb  bl 0x1084d29c
108c4a50  0410a0e1  cpy r1, r4
108c4a54  0700a0e1  cpy r0, r7
108c4a58  3a8cfeeb  bl 0x10867b48
108c4a5c  0620a0e1  cpy r2, r6
108c4a60  0210a0e3  mov r1, #0x2
108c4a64  0700a0e1  cpy r0, r7
108c4a68  b61ffeeb  bl 0x1084c948
108c4a6c  10109fe5  ldr r1, [0x108c4a84]
108c4a70  0800a0e1  cpy r0, r8
108c4a74  0c1080e5  str r1, [r0,#0xc]
108c4a78  0100a0e3  mov r0, #0x1
108c4a7c  000085e5  str r0, [r5,#0x0]
108c4a80  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
