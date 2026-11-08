108b70bc  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b70c0  0060a0e1  cpy r6, r0
108b70c4  10d04de2  sub sp, sp, #0x10
108b70c8  0170a0e1  cpy r7, r1
108b70cc  0380a0e1  cpy r8, r3
108b70d0  0250a0e1  cpy r5, r2
108b70d4  0200a0e1  cpy r0, r2
108b70d8  b853feeb  bl 0x1084bfc0
108b70dc  0040a0e1  cpy r4, r0
108b70e0  0500a0e3  mov r0, #0x5
108b70e4  5453feeb  bl 0x1084be3c
108b70e8  8a10a0e3  mov r1, #0x8a
108b70ec  0c108de5  str r1, [sp,#0xc]
108b70f0  0210a0e3  mov r1, #0x2
108b70f4  04108de5  str r1, [sp,#0x4]
108b70f8  08008de5  str r0, [sp,#0x8]
108b70fc  0830a0e1  cpy r3, r8
108b7100  0520a0e1  cpy r2, r5
108b7104  0710a0e1  cpy r1, r7
108b7108  0600a0e1  cpy r0, r6
108b710c  00408de5  str r4, [sp,#0x0]
108b7110  e652feeb  bl 0x1084bcb0
108b7114  10d08de2  add sp, sp, #0x10
108b7118  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
