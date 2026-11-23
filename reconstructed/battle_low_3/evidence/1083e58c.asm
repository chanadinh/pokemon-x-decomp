1083e58c  f05f2de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
1083e590  0040a0e1  cpy r4, r0
1083e594  0180a0e1  cpy r8, r1
1083e598  0290a0e1  cpy r9, r2
1083e59c  03b0a0e1  cpy r11, r3
1083e5a0  74109fe5  ldr r1, [0x1083e61c]
1083e5a4  d862cde1  ldrd r6, r7, [sp,#0x28]
1083e5a8  70009fe5  ldr r0, [0x1083e620]
1083e5ac  6c2091e5  ldr r2, [r1,#0x6c]
1083e5b0  000052e1  cmp r2, r0
1083e5b4  6c008115  strne r0, [r1,#0x6c]
1083e5b8  060c84e2  add r0, r4, #0x600
1083e5bc  ca0080e2  add r0, r0, #0xca
1083e5c0  ff10a0e3  mov r1, #0xff
1083e5c4  0050a0e1  cpy r5, r0
1083e5c8  d2c500eb  bl 0x1086fd18
1083e5cc  00a0a0e3  mov r10, #0x0
1083e5d0  5da2c4e5  strb r10, [r4,#0x25d]
1083e5d4  c8a6c4e5  strb r10, [r4,#0x6c8]
1083e5d8  c986c4e5  strb r8, [r4,#0x6c9]
1083e5dc  0080c5e5  strb r8, [r5,#0x0]
1083e5e0  d0a684e5  str r10, [r4,#0x6d0]
1083e5e4  0900a0e1  cpy r0, r9
1083e5e8  d4a684e5  str r10, [r4,#0x6d4]
1083e5ec  8409ffeb  bl 0x10800c04
1083e5f0  2c109fe5  ldr r1, [0x1083e624]
1083e5f4  b40081e1  strh r0, [r1,r4]
1083e5f8  010b84e2  add r0, r4, #0x400
1083e5fc  5da2c4e5  strb r10, [r4,#0x25d]
1083e600  bf0f80e2  add r0, r0, #0x2fc
1083e604  b8a684e5  str r10, [r4,#0x6b8]
1083e608  bca684e5  str r10, [r4,#0x6bc]
1083e60c  000a80e8  stmia r0,{r9,r11}
1083e610  1c67c4e5  strb r6, [r4,#0x71c]
1083e614  1d77c4e5  strb r7, [r4,#0x71d]
1083e618  f09fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,pc}
