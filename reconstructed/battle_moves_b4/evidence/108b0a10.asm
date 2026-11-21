108b0a10  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108b0a14  0260a0e1  cpy r6, r2
108b0a18  0080a0e1  cpy r8, r0
108b0a1c  0170a0e1  cpy r7, r1
108b0a20  0300a0e3  mov r0, #0x3
108b0a24  bcebfeeb  bl 0x1086b91c
108b0a28  060050e1  cmp r0, r6
108b0a2c  4500001a  bne 0x108b0b48
108b0a30  0610a0e1  cpy r1, r6
108b0a34  0700a0e1  cpy r0, r7
108b0a38  988efeeb  bl 0x108544a0
108b0a3c  0090a0e1  cpy r9, r0
108b0a40  7884feeb  bl 0x10851c28
108b0a44  000050e3  cmp r0, #0x0
108b0a48  3e00001a  bne 0x108b0b48
108b0a4c  0600a0e3  mov r0, #0x6
108b0a50  b1ebfeeb  bl 0x1086b91c
108b0a54  ff1000e2  and r1, r0, #0xff
108b0a58  0700a0e1  cpy r0, r7
108b0a5c  8f8efeeb  bl 0x108544a0
108b0a60  0050a0e1  cpy r5, r0
108b0a64  646ffeeb  bl 0x1084c7fc
108b0a68  0040a0e1  cpy r4, r0
108b0a6c  0500a0e1  cpy r0, r5
108b0a70  006efeeb  bl 0x1084c278
108b0a74  040050e1  cmp r0, r4
108b0a78  0040a011  cpyne r4, r0
108b0a7c  000054e3  cmp r4, #0x0
108b0a80  3000000a  beq 0x108b0b48
108b0a84  0400a0e1  cpy r0, r4
108b0a88  6dcfffeb  bl 0x108a4844
108b0a8c  000050e3  cmp r0, #0x0
108b0a90  2c00001a  bne 0x108b0b48
108b0a94  0410a0e1  cpy r1, r4
108b0a98  0900a0e1  cpy r0, r9
108b0a9c  e06dfeeb  bl 0x1084c224
108b0aa0  040050e3  cmp r0, #0x4
108b0aa4  2700001a  bne 0x108b0b48
108b0aa8  0800a0e1  cpy r0, r8
108b0aac  b803d0e1  ldrh r0, [r0,#0x38]
108b0ab0  0010a0e1  cpy r1, r0
108b0ab4  0900a0e1  cpy r0, r9
108b0ab8  d96dfeeb  bl 0x1084c224
108b0abc  040050e3  cmp r0, #0x4
108b0ac0  0050a0e1  cpy r5, r0
108b0ac4  1f00000a  beq 0x108b0b48
108b0ac8  0620a0e1  cpy r2, r6
108b0acc  2610a0e3  mov r1, #0x26
108b0ad0  0700a0e1  cpy r0, r7
108b0ad4  28e5feeb  bl 0x10869f7c
108b0ad8  0020a0e3  mov r2, #0x0
108b0adc  0550c0e5  strb r5, [r0,#0x5]
108b0ae0  0620c0e5  strb r2, [r0,#0x6]
108b0ae4  0460c0e5  strb r6, [r0,#0x4]
108b0ae8  0720c0e5  strb r2, [r0,#0x7]
108b0aec  0010a0e1  cpy r1, r0
108b0af0  b840c0e1  strh r4, [r0,#0x8]
108b0af4  0700a0e1  cpy r0, r7
108b0af8  12dcfeeb  bl 0x10867b48
108b0afc  0620a0e1  cpy r2, r6
108b0b00  0310a0e3  mov r1, #0x3
108b0b04  0700a0e1  cpy r0, r7
108b0b08  1be5feeb  bl 0x10869f7c
108b0b0c  0050a0e1  cpy r5, r0
108b0b10  2b2ea0e3  mov r2, #0x2b0
108b0b14  0210a0e3  mov r1, #0x2
108b0b18  040080e2  add r0, r0, #0x4
108b0b1c  ec71feeb  bl 0x1084d2d4
108b0b20  0610a0e1  cpy r1, r6
108b0b24  040085e2  add r0, r5, #0x4
108b0b28  db71feeb  bl 0x1084d29c
108b0b2c  0410a0e1  cpy r1, r4
108b0b30  040085e2  add r0, r5, #0x4
108b0b34  d871feeb  bl 0x1084d29c
108b0b38  0510a0e1  cpy r1, r5
108b0b3c  0700a0e1  cpy r0, r7
108b0b40  f047bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108b0b44  ffdbfeea  b 0x10867b48
108b0b48  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
