108b5440  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b5444  10d04de2  sub sp, sp, #0x10
108b5448  0060a0e1  cpy r6, r0
108b544c  0170a0e1  cpy r7, r1
108b5450  0250a0e1  cpy r5, r2
108b5454  0380a0e1  cpy r8, r3
108b5458  0100a0e3  mov r0, #0x1
108b545c  0040a0e1  cpy r4, r0
108b5460  0500a0e1  cpy r0, r5
108b5464  9259feeb  bl 0x1084bab4
108b5468  a210a0e3  mov r1, #0xa2
108b546c  0c108de5  str r1, [sp,#0xc]
108b5470  0810a0e3  mov r1, #0x8
108b5474  13008de8  stmia sp,{r0,r1,r4}
108b5478  0830a0e1  cpy r3, r8
108b547c  0520a0e1  cpy r2, r5
108b5480  0710a0e1  cpy r1, r7
108b5484  0600a0e1  cpy r0, r6
108b5488  085afeeb  bl 0x1084bcb0
108b548c  10d08de2  add sp, sp, #0x10
108b5490  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
