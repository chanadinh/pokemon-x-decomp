00174e6c  0030a0e3  mov r3,#0x0
00174e70  a10053e1  cmp r3,r1, lsr #0x1
00174e74  1eff2f21  bxcs lr
00174e78  30002de9  stmdb sp!,{r4,r5}
00174e7c  2cc09fe5  ldr r12,[0x174eb0]
00174e80  920c02e0  mul r2,r2,r12
00174e84  83c080e0  add r12,r0,r3, lsl #0x1
00174e88  013083e2  add r3,r3,#0x1
00174e8c  b050dce1  ldrh r5,[r12,#0x0]
00174e90  062a82e2  add r2,r2,#0x6000
00174e94  732082e2  add r2,r2,#0x73
00174e98  a10053e1  cmp r3,r1, lsr #0x1
00174e9c  224825e0  eor r4,r5,r2, lsr #0x10
00174ea0  b040cce1  strh r4,[r12,#0x0]
00174ea4  f4ffff3a  bcc 0x00174e7c
00174ea8  3000bde8  ldmia sp!,{r4,r5}
00174eac  1eff2fe1  bx lr
