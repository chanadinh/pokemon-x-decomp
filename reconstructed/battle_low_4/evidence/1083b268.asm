1083b268  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
1083b26c  0040a0e1  cpy r4, r0
1083b270  0170a0e1  cpy r7, r1
1083b274  0090a0e3  mov r9, #0x0
1083b278  8c109fe5  ldr r1, [0x1083b30c]
1083b27c  8c009fe5  ldr r0, [0x1083b310]
1083b280  ff80a0e3  mov r8, #0xff
1083b284  6c2091e5  ldr r2, [r1,#0x6c]
1083b288  000052e1  cmp r2, r0
1083b28c  6c008115  strne r0, [r1,#0x6c]
1083b290  060c84e2  add r0, r4, #0x600
1083b294  ca0080e2  add r0, r0, #0xca
1083b298  b49684e5  str r9, [r4,#0x6b4]
1083b29c  0050a0e1  cpy r5, r0
1083b2a0  0810a0e1  cpy r1, r8
1083b2a4  b89684e5  str r9, [r4,#0x6b8]
1083b2a8  9ad200eb  bl 0x1086fd18
1083b2ac  c886c4e5  strb r8, [r4,#0x6c8]
1083b2b0  c986c4e5  strb r8, [r4,#0x6c9]
1083b2b4  5d92c4e5  strb r9, [r4,#0x25d]
1083b2b8  000094e5  ldr r0, [r4,#0x0]
1083b2bc  016b84e2  add r6, r4, #0x400
1083b2c0  b26f86e2  add r6, r6, #0x2c8
1083b2c4  0710a0e1  cpy r1, r7
1083b2c8  0c2190e5  ldr r2, [r0,#0x10c]
1083b2cc  0400a0e1  cpy r0, r4
1083b2d0  32ff2fe1  blx r2
1083b2d4  0000c6e5  strb r0, [r6,#0x0]
1083b2d8  000094e5  ldr r0, [r4,#0x0]
1083b2dc  0710a0e1  cpy r1, r7
1083b2e0  0c2190e5  ldr r2, [r0,#0x10c]
1083b2e4  0400a0e1  cpy r0, r4
1083b2e8  32ff2fe1  blx r2
1083b2ec  0000c5e5  strb r0, [r5,#0x0]
1083b2f0  000094e5  ldr r0, [r4,#0x0]
1083b2f4  bc1090e5  ldr r1, [r0,#0xbc]
1083b2f8  0400a0e1  cpy r0, r4
1083b2fc  31ff2fe1  blx r1
1083b300  000050e3  cmp r0, #0x0
1083b304  b8868415  strne r8, [r4,#0x6b8]
1083b308  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
