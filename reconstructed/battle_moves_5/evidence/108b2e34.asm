108b2e34  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b2e38  0060a0e1  cpy r6, r0
108b2e3c  10d04de2  sub sp, sp, #0x10
108b2e40  0170a0e1  cpy r7, r1
108b2e44  0250a0e1  cpy r5, r2
108b2e48  0380a0e1  cpy r8, r3
108b2e4c  0500a0e3  mov r0, #0x5
108b2e50  f963feeb  bl 0x1084be3c
108b2e54  0040a0e1  cpy r4, r0
108b2e58  0500a0e1  cpy r0, r5
108b2e5c  5764feeb  bl 0x1084bfc0
108b2e60  8e10a0e3  mov r1, #0x8e
108b2e64  0c108de5  str r1, [sp,#0xc]
108b2e68  0310a0e3  mov r1, #0x3
108b2e6c  13008de8  stmia sp,{r0,r1,r4}
108b2e70  0830a0e1  cpy r3, r8
108b2e74  0520a0e1  cpy r2, r5
108b2e78  0710a0e1  cpy r1, r7
108b2e7c  0600a0e1  cpy r0, r6
108b2e80  8a63feeb  bl 0x1084bcb0
108b2e84  10d08de2  add sp, sp, #0x10
108b2e88  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
