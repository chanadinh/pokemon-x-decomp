108b0db4  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b0db8  0060a0e1  cpy r6, r0
108b0dbc  10d04de2  sub sp, sp, #0x10
108b0dc0  0170a0e1  cpy r7, r1
108b0dc4  0250a0e1  cpy r5, r2
108b0dc8  0380a0e1  cpy r8, r3
108b0dcc  0500a0e3  mov r0, #0x5
108b0dd0  196cfeeb  bl 0x1084be3c
108b0dd4  0040a0e1  cpy r4, r0
108b0dd8  0500a0e1  cpy r0, r5
108b0ddc  776cfeeb  bl 0x1084bfc0
108b0de0  9610a0e3  mov r1, #0x96
108b0de4  0c108de5  str r1, [sp,#0xc]
108b0de8  0510a0e3  mov r1, #0x5
108b0dec  13008de8  stmia sp,{r0,r1,r4}
108b0df0  0830a0e1  cpy r3, r8
108b0df4  0520a0e1  cpy r2, r5
108b0df8  0710a0e1  cpy r1, r7
108b0dfc  0600a0e1  cpy r0, r6
108b0e00  aa6bfeeb  bl 0x1084bcb0
108b0e04  10d08de2  add sp, sp, #0x10
108b0e08  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
