1083b9f0  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
1083b9f4  0040a0e1  cpy r4, r0
1083b9f8  0170a0e1  cpy r7, r1
1083b9fc  ff80a0e3  mov r8, #0xff
1083ba00  c4109fe5  ldr r1, [0x1083bacc]
1083ba04  c4009fe5  ldr r0, [0x1083bad0]
1083ba08  6c2091e5  ldr r2, [r1,#0x6c]
1083ba0c  000052e1  cmp r2, r0
1083ba10  6c008115  strne r0, [r1,#0x6c]
1083ba14  060c84e2  add r0, r4, #0x600
1083ba18  ca0080e2  add r0, r0, #0xca
1083ba1c  0810a0e1  cpy r1, r8
1083ba20  bcd000eb  bl 0x1086fd18
1083ba24  c886c4e5  strb r8, [r4,#0x6c8]
1083ba28  0090a0e3  mov r9, #0x0
1083ba2c  c986c4e5  strb r8, [r4,#0x6c9]
1083ba30  5d92c4e5  strb r9, [r4,#0x25d]
1083ba34  000094e5  ldr r0, [r4,#0x0]
1083ba38  015b84e2  add r5, r4, #0x400
1083ba3c  066c84e2  add r6, r4, #0x600
1083ba40  b25f85e2  add r5, r5, #0x2c8
1083ba44  0c2190e5  ldr r2, [r0,#0x10c]
1083ba48  c96086e2  add r6, r6, #0xc9
1083ba4c  0710a0e1  cpy r1, r7
1083ba50  0400a0e1  cpy r0, r4
1083ba54  32ff2fe1  blx r2
1083ba58  0000c5e5  strb r0, [r5,#0x0]
1083ba5c  000094e5  ldr r0, [r4,#0x0]
1083ba60  0710a0e1  cpy r1, r7
1083ba64  0c2190e5  ldr r2, [r0,#0x10c]
1083ba68  0400a0e1  cpy r0, r4
1083ba6c  32ff2fe1  blx r2
1083ba70  0000c6e5  strb r0, [r6,#0x0]
1083ba74  0000d5e5  ldrb r0, [r5,#0x0]
1083ba78  000184e0  add r0, r4, r0, lsl #0x2
1083ba7c  000190e5  ldr r0, [r0,#0x100]
1083ba80  001090e5  ldr r1, [r0,#0x0]
1083ba84  7c1091e5  ldr r1, [r1,#0x7c]
1083ba88  31ff2fe1  blx r1
1083ba8c  000050e3  cmp r0, #0x0
1083ba90  0700001a  bne 0x1083bab4
1083ba94  c806d4e5  ldrb r0, [r4,#0x6c8]
1083ba98  000184e0  add r0, r4, r0, lsl #0x2
1083ba9c  000190e5  ldr r0, [r0,#0x100]
1083baa0  5c01d0e5  ldrb r0, [r0,#0x15c]
1083baa4  000050e3  cmp r0, #0x0
1083baa8  6400a003  moveq r0, #0x64
1083baac  b8068405  streq r0, [r4,#0x6b8]
1083bab0  0400000a  beq 0x1083bac8
1083bab4  0400a0e1  cpy r0, r4
1083bab8  b89684e5  str r9, [r4,#0x6b8]
1083babc  f047bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
1083bac0  0c109fe5  ldr r1, [0x1083bad4]
1083bac4  667600ea  b 0x10859464
1083bac8  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
