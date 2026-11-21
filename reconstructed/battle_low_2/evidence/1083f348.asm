1083f348  54c09fe5  ldr r12, [0x1083f3a4]
1083f34c  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
1083f350  0160a0e1  cpy r6, r1
1083f354  0070a0e3  mov r7, #0x0
1083f358  48309fe5  ldr r3, [0x1083f3a8]
1083f35c  6c409ce5  ldr r4, [r12,#0x6c]
1083f360  0250a0e1  cpy r5, r2
1083f364  030054e1  cmp r4, r3
1083f368  6c308c15  strne r3, [r12,#0x6c]
1083f36c  647780e5  str r7, [r0,#0x764]
1083f370  904190e5  ldr r4, [r0,#0x190]
1083f374  0400a0e1  cpy r0, r4
1083f378  755d00eb  bl 0x10856954
1083f37c  0100a0e3  mov r0, #0x1
1083f380  0870c4e5  strb r7, [r4,#0x8]
1083f384  c901c4e5  strb r0, [r4,#0x1c9]
1083f388  ca61c4e5  strb r6, [r4,#0x1ca]
1083f38c  cb51c4e5  strb r5, [r4,#0x1cb]
1083f390  cc01c4e5  strb r0, [r4,#0x1cc]
1083f394  711f84e2  add r1, r4, #0x1c4
1083f398  050d84e2  add r0, r4, #0x140
1083f39c  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
1083f3a0  9b2b00ea  b 0x1084a214
