1084d184  70402de9  stmdb sp!,{r4,r5,r6,lr}
1084d188  0050a0e1  cpy r5, r0
1084d18c  5201d0e5  ldrb r0, [r0,#0x152]
1084d190  0140a0e1  cpy r4, r1
1084d194  040050e1  cmp r0, r4
1084d198  0300008a  bhi 0x1084d1ac
1084d19c  0020a0e3  mov r2, #0x0
1084d1a0  0210a0e1  cpy r1, r2
1084d1a4  0200a0e1  cpy r0, r2
1084d1a8  79ccfeeb  bl 0x10800394
1084d1ac  840164e0  rsb r0, r4, r4, lsl #0x3
1084d1b0  800085e0  add r0, r5, r0, lsl #0x1
1084d1b4  1e11d0e5  ldrb r1, [r0,#0x11e]
1084d1b8  1f01d0e5  ldrb r0, [r0,#0x11f]
1084d1bc  010040e0  sub r0, r0, r1
1084d1c0  ff0000e2  and r0, r0, #0xff
1084d1c4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
