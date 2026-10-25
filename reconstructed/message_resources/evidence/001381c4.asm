001381c4  30402de9  stmdb sp!,{r4,r5,lr}
001381c8  14d04de2  sub sp, sp, #0x14
001381cc  0250a0e1  cpy r5, r2
001381d0  0040a0e1  cpy r4, r0
001381d4  04208de2  add r2, sp, #0x4
001381d8  bbbeffeb  bl 0x00127ccc
001381dc  0c009de5  ldr r0, [sp,#0xc]
001381e0  0010a0e3  mov r1, #0x0
001381e4  00108de5  str r1, [sp,#0x0]
001381e8  052080e0  add r2, r0, r5
001381ec  180084e2  add r0, r4, #0x18
001381f0  c23fa0e1  mov r3, r2, asr #0x1f
001381f4  30edffeb  bl 0x001336bc
001381f8  021110e2  ands r1, r0, #0x80000000
001381fc  0200005a  bpl 0x0013820c
00138200  0f10a0e1  cpy r1, pc
00138204  a02fb0e1  movs r2, r0, lsr #0x1f
00138208  bdb0ff1b  blne 0x00124504
0013820c  14d08de2  add sp, sp, #0x14
00138210  3080bde8  ldmia sp!,{r4,r5,pc}
