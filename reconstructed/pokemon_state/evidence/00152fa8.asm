00152fa8  70402de9  stmdb sp!,{r4,r5,r6,lr}
00152fac  0050a0e1  cpy r5,r0
00152fb0  aa5500eb  bl 0x00168660
00152fb4  0010a0e3  mov r1,#0x0
00152fb8  0500a0e1  cpy r0,r5
00152fbc  e56000eb  bl 0x0016b358
00152fc0  1500d0e5  ldrb r0,[r0,#0x15]
00152fc4  a041a0e1  mov r4,r0, lsr #0x3
00152fc8  0500a0e1  cpy r0,r5
00152fcc  875500eb  bl 0x001685f0
00152fd0  0400a0e1  cpy r0,r4
00152fd4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
