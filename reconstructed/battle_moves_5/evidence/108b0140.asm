108b0140  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b0144  10d04de2  sub sp, sp, #0x10
108b0148  0060a0e1  cpy r6, r0
108b014c  0170a0e1  cpy r7, r1
108b0150  0250a0e1  cpy r5, r2
108b0154  0380a0e1  cpy r8, r3
108b0158  0100a0e3  mov r0, #0x1
108b015c  0040a0e1  cpy r4, r0
108b0160  0500a0e1  cpy r0, r5
108b0164  526efeeb  bl 0x1084bab4
108b0168  9e10a0e3  mov r1, #0x9e
108b016c  0c108de5  str r1, [sp,#0xc]
108b0170  0710a0e3  mov r1, #0x7
108b0174  13008de8  stmia sp,{r0,r1,r4}
108b0178  0830a0e1  cpy r3, r8
108b017c  0520a0e1  cpy r2, r5
108b0180  0710a0e1  cpy r1, r7
108b0184  0600a0e1  cpy r0, r6
108b0188  c86efeeb  bl 0x1084bcb0
108b018c  10d08de2  add sp, sp, #0x10
108b0190  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
