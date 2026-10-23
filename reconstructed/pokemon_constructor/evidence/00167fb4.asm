00167fb4  0030a0e3  mov r3,#0x0
00167fb8  a10053e1  cmp r3,r1, lsr #0x1
00167fbc  1eff2f21  bxcs lr
00167fc0  30002de9  stmdb sp!,{r4,r5}
00167fc4  2cc09fe5  ldr r12,[0x167ff8]
00167fc8  920c02e0  mul r2,r2,r12
00167fcc  83c080e0  add r12,r0,r3, lsl #0x1
00167fd0  013083e2  add r3,r3,#0x1
00167fd4  b050dce1  ldrh r5,[r12,#0x0]
00167fd8  062a82e2  add r2,r2,#0x6000
00167fdc  732082e2  add r2,r2,#0x73
00167fe0  a10053e1  cmp r3,r1, lsr #0x1
00167fe4  224825e0  eor r4,r5,r2, lsr #0x10
00167fe8  b040cce1  strh r4,[r12,#0x0]
00167fec  f4ffff3a  bcc 0x00167fc4
00167ff0  3000bde8  ldmia sp!,{r4,r5}
00167ff4  1eff2fe1  bx lr
