108b2d6c  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b2d70  0060a0e1  cpy r6, r0
108b2d74  10d04de2  sub sp, sp, #0x10
108b2d78  0170a0e1  cpy r7, r1
108b2d7c  0250a0e1  cpy r5, r2
108b2d80  0380a0e1  cpy r8, r3
108b2d84  0500a0e3  mov r0, #0x5
108b2d88  2b64feeb  bl 0x1084be3c
108b2d8c  0040a0e1  cpy r4, r0
108b2d90  0500a0e1  cpy r0, r5
108b2d94  8964feeb  bl 0x1084bfc0
108b2d98  8210a0e3  mov r1, #0x82
108b2d9c  0c108de5  str r1, [sp,#0xc]
108b2da0  0010a0e3  mov r1, #0x0
108b2da4  13008de8  stmia sp,{r0,r1,r4}
108b2da8  0830a0e1  cpy r3, r8
108b2dac  0520a0e1  cpy r2, r5
108b2db0  0710a0e1  cpy r1, r7
108b2db4  0600a0e1  cpy r0, r6
108b2db8  bc63feeb  bl 0x1084bcb0
108b2dbc  10d08de2  add sp, sp, #0x10
108b2dc0  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
