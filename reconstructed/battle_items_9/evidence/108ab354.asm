108ab354  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108ab358  0060a0e1  cpy r6, r0
108ab35c  0150a0e1  cpy r5, r1
108ab360  0270a0e1  cpy r7, r2
108ab364  0340a0e1  cpy r4, r3
108ab368  0400a0e3  mov r0, #0x4
108ab36c  6a01ffeb  bl 0x1086b91c
108ab370  070050e1  cmp r0, r7
108ab374  1100001a  bne 0x108ab3c0
108ab378  0600a0e1  cpy r0, r6
108ab37c  0370a0e3  mov r7, #0x3
108ab380  b803d0e1  ldrh r0, [r0,#0x38]
108ab384  0710a0e1  cpy r1, r7
108ab388  f986feeb  bl 0x1084cf74
108ab38c  ff0000e2  and r0, r0, #0xff
108ab390  5883feeb  bl 0x1084c0f8
108ab394  000050e3  cmp r0, #0x0
108ab398  0400001a  bne 0x108ab3b0
108ab39c  0210a0e3  mov r1, #0x2
108ab3a0  0500a0e1  cpy r0, r5
108ab3a4  5083feeb  bl 0x1084c0ec
108ab3a8  000050e3  cmp r0, #0x0
108ab3ac  0200000a  beq 0x108ab3bc
108ab3b0  0310a0e3  mov r1, #0x3
108ab3b4  3b00a0e3  mov r0, #0x3b
108ab3b8  45e9feeb  bl 0x108658d4
108ab3bc  000084e5  str r0, [r4,#0x0]
108ab3c0  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
