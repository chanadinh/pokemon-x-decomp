1086b5e0  0f002de9  stmdb sp!,{r0,r1,r2,r3}
1086b5e4  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
1086b5e8  0040a0e1  cpy r4, r0
1086b5ec  2c608de2  add r6, sp, #0x2c
1086b5f0  28109de5  ldr r1, [sp,#0x28]
1086b5f4  7150ffe6  uxth r5, r1
1086b5f8  2410dde5  ldrb r1, [sp,#0x24]
1086b5fc  9ddaffeb  bl 0x10862078
1086b600  0510a0e1  cpy r1, r5
1086b604  0400a0e1  cpy r0, r4
1086b608  9adaffeb  bl 0x10862078
1086b60c  2400dde5  ldrb r0, [sp,#0x24]
1086b610  6b0050e3  cmp r0, #0x6b
1086b614  0300001a  bne 0x1086b628
1086b618  2c109de5  ldr r1, [sp,#0x2c]
1086b61c  046086e2  add r6, r6, #0x4
1086b620  0400a0e1  cpy r0, r4
1086b624  5189ffeb  bl 0x1084db70
1086b628  7c909fe5  ldr r9, [0x1086b6ac]
1086b62c  087084e2  add r7, r4, #0x8
1086b630  000094e5  ldr r0, [r4,#0x0]
1086b634  045096e4  ldr r5, [r6],#0x4
1086b638  090050e1  cmp r0, r9
1086b63c  0300003a  bcc 0x1086b650
1086b640  0020a0e3  mov r2, #0x0
1086b644  0210a0e1  cpy r1, r2
1086b648  0200a0e1  cpy r0, r2
1086b64c  5453feeb  bl 0x108003a4
1086b650  000094e5  ldr r0, [r4,#0x0]
1086b654  253ca0e1  mov r3, r5, lsr #0x18
1086b658  2518a0e1  mov r1, r5, lsr #0x10
1086b65c  01c080e2  add r12, r0, #0x1
1086b660  00c084e5  str r12, [r4,#0x0]
1086b664  0730c0e7  strb r3, [r0,r7]
1086b668  003094e5  ldr r3, [r4,#0x0]
1086b66c  2524a0e1  mov r2, r5, lsr #0x8
1086b670  010875e3  cmn r5, #0x10000
1086b674  01c083e2  add r12, r3, #0x1
1086b678  00c084e5  str r12, [r4,#0x0]
1086b67c  0710c3e7  strb r1, [r3,r7]
1086b680  001094e5  ldr r1, [r4,#0x0]
1086b684  01c081e2  add r12, r1, #0x1
1086b688  00c084e5  str r12, [r4,#0x0]
1086b68c  0720c1e7  strb r2, [r1,r7]
1086b690  001094e5  ldr r1, [r4,#0x0]
1086b694  013081e2  add r3, r1, #0x1
1086b698  003084e5  str r3, [r4,#0x0]
1086b69c  0750c1e7  strb r5, [r1,r7]
1086b6a0  e2ffff1a  bne 0x1086b630
1086b6a4  f007bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10}
1086b6a8  14f09de4  ldr pc, [sp],#0x14
