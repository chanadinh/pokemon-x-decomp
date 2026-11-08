108b5074  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b5078  10d04de2  sub sp, sp, #0x10
108b507c  0060a0e1  cpy r6, r0
108b5080  0170a0e1  cpy r7, r1
108b5084  0250a0e1  cpy r5, r2
108b5088  0380a0e1  cpy r8, r3
108b508c  0100a0e3  mov r0, #0x1
108b5090  0040a0e1  cpy r4, r0
108b5094  0500a0e1  cpy r0, r5
108b5098  855afeeb  bl 0x1084bab4
108b509c  d610a0e3  mov r1, #0xd6
108b50a0  0c108de5  str r1, [sp,#0xc]
108b50a4  0e10a0e3  mov r1, #0xe
108b50a8  13008de8  stmia sp,{r0,r1,r4}
108b50ac  0830a0e1  cpy r3, r8
108b50b0  0520a0e1  cpy r2, r5
108b50b4  0710a0e1  cpy r1, r7
108b50b8  0600a0e1  cpy r0, r6
108b50bc  fb5afeeb  bl 0x1084bcb0
108b50c0  10d08de2  add sp, sp, #0x10
108b50c4  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
