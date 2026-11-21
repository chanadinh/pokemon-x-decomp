1081d2b0  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
1081d2b4  08d04de2  sub sp, sp, #0x8
1081d2b8  0050a0e1  cpy r5, r0
1081d2bc  0180a0e1  cpy r8, r1
1081d2c0  0260a0e1  cpy r6, r2
1081d2c4  012901eb  bl 0x108676d0
1081d2c8  0070a0e1  cpy r7, r0
1081d2cc  1980d8e5  ldrb r8, [r8,#0x19]
1081d2d0  0040a0e3  mov r4, #0x0
1081d2d4  0d10a0e1  cpy r1, sp
1081d2d8  0500a0e1  cpy r0, r5
1081d2dc  f82001eb  bl 0x108656c4
1081d2e0  04208de2  add r2, sp, #0x4
1081d2e4  0d10a0e1  cpy r1, sp
1081d2e8  0500a0e1  cpy r0, r5
1081d2ec  9d2001eb  bl 0x10865568
1081d2f0  000050e3  cmp r0, #0x0
1081d2f4  00f020e3  nop
1081d2f8  0a00000a  beq 0x1081d328
1081d2fc  04109de5  ldr r1, [sp,#0x4]
1081d300  1900d1e5  ldrb r0, [r1,#0x19]
1081d304  080050e1  cmp r0, r8
1081d308  f4ffff0a  beq 0x1081d2e0
1081d30c  0500a0e1  cpy r0, r5
1081d310  0620a0e1  cpy r2, r6
1081d314  ed2801eb  bl 0x108676d0
1081d318  070050e1  cmp r0, r7
1081d31c  01008482  addhi r0, r4, #0x1
1081d320  7040ff86  uxthhi r4, r0
1081d324  edffffea  b 0x1081d2e0
1081d328  08d08de2  add sp, sp, #0x8
1081d32c  0400a0e1  cpy r0, r4
1081d330  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
