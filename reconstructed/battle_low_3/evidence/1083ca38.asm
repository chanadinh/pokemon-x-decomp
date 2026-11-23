1083ca38  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
1083ca3c  0050a0e1  cpy r5, r0
1083ca40  0370a0e1  cpy r7, r3
1083ca44  6cc09fe5  ldr r12, [0x1083cab8]
1083ca48  6c009fe5  ldr r0, [0x1083cabc]
1083ca4c  18409de5  ldr r4, [sp,#0x18]
1083ca50  0280a0e1  cpy r8, r2
1083ca54  6c609ce5  ldr r6, [r12,#0x6c]
1083ca58  000056e1  cmp r6, r0
1083ca5c  6c008c15  strne r0, [r12,#0x6c]
1083ca60  760e85e2  add r0, r5, #0x760
1083ca64  0160a0e1  cpy r6, r1
1083ca68  120080e8  stmia r0,{r1,r4}
1083ca6c  904195e5  ldr r4, [r5,#0x190]
1083ca70  0400a0e1  cpy r0, r4
1083ca74  b66700eb  bl 0x10856954
1083ca78  0000a0e3  mov r0, #0x0
1083ca7c  0800c4e5  strb r0, [r4,#0x8]
1083ca80  c901c4e5  strb r0, [r4,#0x1c9]
1083ca84  c46184e5  str r6, [r4,#0x1c4]
1083ca88  0110a0e3  mov r1, #0x1
1083ca8c  c871c4e5  strb r7, [r4,#0x1c8]
1083ca90  cc11c4e5  strb r1, [r4,#0x1cc]
1083ca94  711f84e2  add r1, r4, #0x1c4
1083ca98  050d84e2  add r0, r4, #0x140
1083ca9c  d08184e5  str r8, [r4,#0x1d0]
1083caa0  db3500eb  bl 0x1084a214
1083caa4  050d84e2  add r0, r4, #0x140
1083caa8  833500eb  bl 0x1084a0bc
1083caac  900195e5  ldr r0, [r5,#0x190]
1083cab0  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
1083cab4  3e6b00ea  b 0x108577b4
