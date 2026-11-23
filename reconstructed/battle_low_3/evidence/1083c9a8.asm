1083c9a8  f0402de9  stmdb sp!,{r4,r5,r6,r7,lr}
1083c9ac  0040a0e1  cpy r4, r0
1083c9b0  0170a0e1  cpy r7, r1
1083c9b4  74109fe5  ldr r1, [0x1083ca30]
1083c9b8  0260a0e1  cpy r6, r2
1083c9bc  70009fe5  ldr r0, [0x1083ca34]
1083c9c0  0cd04de2  sub sp, sp, #0xc
1083c9c4  6c2091e5  ldr r2, [r1,#0x6c]
1083c9c8  0350a0e1  cpy r5, r3
1083c9cc  000052e1  cmp r2, r0
1083c9d0  6c008115  strne r0, [r1,#0x6c]
1083c9d4  000094e5  ldr r0, [r4,#0x0]
1083c9d8  bc1090e5  ldr r1, [r0,#0xbc]
1083c9dc  0400a0e1  cpy r0, r4
1083c9e0  31ff2fe1  blx r1
1083c9e4  000050e3  cmp r0, #0x0
1083c9e8  0100a013  movne r0, #0x1
1083c9ec  0d00001a  bne 0x1083ca28
1083c9f0  900194e5  ldr r0, [r4,#0x190]
1083c9f4  7710ffe6  uxth r1, r7
1083c9f8  7620afe6  sxtb r2, r6
1083c9fc  b00790e5  ldr r0, [r0,#0x7b0]
1083ca00  138401eb  bl 0x1089da54
1083ca04  900194e5  ldr r0, [r4,#0x190]
1083ca08  0020a0e3  mov r2, #0x0
1083ca0c  0530a0e1  cpy r3, r5
1083ca10  b01790e5  ldr r1, [r0,#0x7b0]
1083ca14  3a0e80e2  add r0, r0, #0x3a0
1083ca18  00208de5  str r2, [sp,#0x0]
1083ca1c  04208de5  str r2, [sp,#0x4]
1083ca20  ca6d00eb  bl 0x10858150
1083ca24  0100a0e3  mov r0, #0x1
1083ca28  0cd08de2  add sp, sp, #0xc
1083ca2c  f080bde8  ldmia sp!,{r4,r5,r6,r7,pc}
