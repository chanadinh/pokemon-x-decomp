1083c770  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
1083c774  0040a0e1  cpy r4, r0
1083c778  0170a0e1  cpy r7, r1
1083c77c  ff80a0e3  mov r8, #0xff
1083c780  c4109fe5  ldr r1, [0x1083c84c]
1083c784  c4009fe5  ldr r0, [0x1083c850]
1083c788  6c2091e5  ldr r2, [r1,#0x6c]
1083c78c  000052e1  cmp r2, r0
1083c790  6c008115  strne r0, [r1,#0x6c]
1083c794  060c84e2  add r0, r4, #0x600
1083c798  ca0080e2  add r0, r0, #0xca
1083c79c  0810a0e1  cpy r1, r8
1083c7a0  5ccd00eb  bl 0x1086fd18
1083c7a4  c886c4e5  strb r8, [r4,#0x6c8]
1083c7a8  0090a0e3  mov r9, #0x0
1083c7ac  c986c4e5  strb r8, [r4,#0x6c9]
1083c7b0  5d92c4e5  strb r9, [r4,#0x25d]
1083c7b4  000094e5  ldr r0, [r4,#0x0]
1083c7b8  015b84e2  add r5, r4, #0x400
1083c7bc  066c84e2  add r6, r4, #0x600
1083c7c0  b25f85e2  add r5, r5, #0x2c8
1083c7c4  0c2190e5  ldr r2, [r0,#0x10c]
1083c7c8  c96086e2  add r6, r6, #0xc9
1083c7cc  0710a0e1  cpy r1, r7
1083c7d0  0400a0e1  cpy r0, r4
1083c7d4  32ff2fe1  blx r2
1083c7d8  0000c5e5  strb r0, [r5,#0x0]
1083c7dc  000094e5  ldr r0, [r4,#0x0]
1083c7e0  0710a0e1  cpy r1, r7
1083c7e4  0c2190e5  ldr r2, [r0,#0x10c]
1083c7e8  0400a0e1  cpy r0, r4
1083c7ec  32ff2fe1  blx r2
1083c7f0  0000c6e5  strb r0, [r6,#0x0]
1083c7f4  0000d5e5  ldrb r0, [r5,#0x0]
1083c7f8  000184e0  add r0, r4, r0, lsl #0x2
1083c7fc  000190e5  ldr r0, [r0,#0x100]
1083c800  001090e5  ldr r1, [r0,#0x0]
1083c804  7c1091e5  ldr r1, [r1,#0x7c]
1083c808  31ff2fe1  blx r1
1083c80c  000050e3  cmp r0, #0x0
1083c810  0700001a  bne 0x1083c834
1083c814  c806d4e5  ldrb r0, [r4,#0x6c8]
1083c818  000184e0  add r0, r4, r0, lsl #0x2
1083c81c  000190e5  ldr r0, [r0,#0x100]
1083c820  5c01d0e5  ldrb r0, [r0,#0x15c]
1083c824  000050e3  cmp r0, #0x0
1083c828  6400a003  moveq r0, #0x64
1083c82c  b8068405  streq r0, [r4,#0x6b8]
1083c830  0400000a  beq 0x1083c848
1083c834  0400a0e1  cpy r0, r4
1083c838  b89684e5  str r9, [r4,#0x6b8]
1083c83c  f047bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
1083c840  271ea0e3  mov r1, #0x270
1083c844  067300ea  b 0x10859464
1083c848  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
