00174eb4  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
00174eb8  0050a0e1  cpy r5, r0
00174ebc  0170a0e1  cpy r7, r1
00174ec0  0100a0e1  cpy r0, r1
00174ec4  b400d0e1  ldrh r0, [r0,#0x4]
00174ec8  0060a0e1  cpy r6, r0
00174ecc  0800d5e5  ldrb r0, [r5,#0x8]
00174ed0  0040a0e3  mov r4, #0x0
00174ed4  000050e3  cmp r0, #0x0
00174ed8  0900009a  bls 0x00174f04
00174edc  040095e5  ldr r0, [r5,#0x4]
00174ee0  040190e7  ldr r0, [r0,r4,lsl #0x2]
00174ee4  b400d0e1  ldrh r0, [r0,#0x4]
00174ee8  060050e1  cmp r0, r6
00174eec  00f020e3  nop
00174ef0  0d00000a  beq 0x00174f2c
00174ef4  0800d5e5  ldrb r0, [r5,#0x8]
00174ef8  014084e2  add r4, r4, #0x1
00174efc  040050e1  cmp r0, r4
00174f00  f5ffff8a  bhi 0x00174edc
00174f04  040095e5  ldr r0, [r5,#0x4]
00174f08  0910d5e5  ldrb r1, [r5,#0x9]
00174f0c  011190e7  ldr r1, [r0,r1,lsl #0x2]
00174f10  0700a0e1  cpy r0, r7
00174f14  af1d00eb  bl 0x0017c5d8
00174f18  0900d5e5  ldrb r0, [r5,#0x9]
00174f1c  0810d5e5  ldrb r1, [r5,#0x8]
00174f20  010080e2  add r0, r0, #0x1
00174f24  8ba6ffeb  bl 0x0015e958
00174f28  0910c5e5  strb r1, [r5,#0x9]
00174f2c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
