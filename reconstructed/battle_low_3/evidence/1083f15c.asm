1083f15c  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
1083f160  0150a0e1  cpy r5, r1
1083f164  0040a0e1  cpy r4, r0
1083f168  7c109fe5  ldr r1, [0x1083f1ec]
1083f16c  0260a0e1  cpy r6, r2
1083f170  78009fe5  ldr r0, [0x1083f1f0]
1083f174  6c2091e5  ldr r2, [r1,#0x6c]
1083f178  000052e1  cmp r2, r0
1083f17c  6c008115  strne r0, [r1,#0x6c]
1083f180  900194e5  ldr r0, [r4,#0x190]
1083f184  8a6100eb  bl 0x108577b4
1083f188  900194e5  ldr r0, [r4,#0x190]
1083f18c  010b80e2  add r0, r0, #0x400
1083f190  dc0080e2  add r0, r0, #0xdc
1083f194  0070a0e1  cpy r7, r0
1083f198  a4b9ffeb  bl 0x1082d830
1083f19c  0620a0e1  cpy r2, r6
1083f1a0  0510a0e1  cpy r1, r5
1083f1a4  0700a0e1  cpy r0, r7
1083f1a8  4ab9ffeb  bl 0x1082d6d8
1083f1ac  001094e5  ldr r1, [r4,#0x0]
1083f1b0  0400a0e1  cpy r0, r4
1083f1b4  0668a0e3  mov r6, #0x60000
1083f1b8  0070a0e3  mov r7, #0x0
1083f1bc  c01091e5  ldr r1, [r1,#0xc0]
1083f1c0  0050e0e3  mvn r5, #0x0
1083f1c4  31ff2fe1  blx r1
1083f1c8  000050e3  cmp r0, #0x0
1083f1cc  0500001a  bne 0x1083f1e8
1083f1d0  0730a0e1  cpy r3, r7
1083f1d4  0520a0e1  cpy r2, r5
1083f1d8  0600a0e1  cpy r0, r6
1083f1dc  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
1083f1e0  0010a0e3  mov r1, #0x0
1083f1e4  1804ffea  b 0x1080024c
1083f1e8  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
