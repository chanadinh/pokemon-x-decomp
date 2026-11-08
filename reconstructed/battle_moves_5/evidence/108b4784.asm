108b4784  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b4788  0060a0e1  cpy r6, r0
108b478c  10d04de2  sub sp, sp, #0x10
108b4790  0170a0e1  cpy r7, r1
108b4794  0250a0e1  cpy r5, r2
108b4798  0380a0e1  cpy r8, r3
108b479c  0100a0e3  mov r0, #0x1
108b47a0  a55dfeeb  bl 0x1084be3c
108b47a4  0040a0e1  cpy r4, r0
108b47a8  0500a0e1  cpy r0, r5
108b47ac  035efeeb  bl 0x1084bfc0
108b47b0  d310a0e3  mov r1, #0xd3
108b47b4  0c108de5  str r1, [sp,#0xc]
108b47b8  1010a0e3  mov r1, #0x10
108b47bc  13008de8  stmia sp,{r0,r1,r4}
108b47c0  0830a0e1  cpy r3, r8
108b47c4  0520a0e1  cpy r2, r5
108b47c8  0710a0e1  cpy r1, r7
108b47cc  0600a0e1  cpy r0, r6
108b47d0  365dfeeb  bl 0x1084bcb0
108b47d4  10d08de2  add sp, sp, #0x10
108b47d8  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
