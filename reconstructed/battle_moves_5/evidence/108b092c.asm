108b092c  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b0930  10d04de2  sub sp, sp, #0x10
108b0934  0060a0e1  cpy r6, r0
108b0938  0170a0e1  cpy r7, r1
108b093c  0250a0e1  cpy r5, r2
108b0940  0380a0e1  cpy r8, r3
108b0944  0100a0e3  mov r0, #0x1
108b0948  0040a0e1  cpy r4, r0
108b094c  0500a0e1  cpy r0, r5
108b0950  576cfeeb  bl 0x1084bab4
108b0954  9a10a0e3  mov r1, #0x9a
108b0958  0c108de5  str r1, [sp,#0xc]
108b095c  0610a0e3  mov r1, #0x6
108b0960  13008de8  stmia sp,{r0,r1,r4}
108b0964  0830a0e1  cpy r3, r8
108b0968  0520a0e1  cpy r2, r5
108b096c  0710a0e1  cpy r1, r7
108b0970  0600a0e1  cpy r0, r6
108b0974  cd6cfeeb  bl 0x1084bcb0
108b0978  10d08de2  add sp, sp, #0x10
108b097c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
