108b2168  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b216c  0050a0e1  cpy r5, r0
108b2170  0170a0e1  cpy r7, r1
108b2174  0260a0e1  cpy r6, r2
108b2178  0340a0e1  cpy r4, r3
108b217c  0200a0e3  mov r0, #0x2
108b2180  e5e5feeb  bl 0x1086b91c
108b2184  060050e1  cmp r0, r6
108b2188  1b00001a  bne 0x108b21fc
108b218c  1200a0e3  mov r0, #0x12
108b2190  e1e5feeb  bl 0x1086b91c
108b2194  0080a0e1  cpy r8, r0
108b2198  0500a0e1  cpy r0, r5
108b219c  b803d0e1  ldrh r0, [r0,#0x38]
108b21a0  000058e1  cmp r8, r0
108b21a4  1400001a  bne 0x108b21fc
108b21a8  0620a0e1  cpy r2, r6
108b21ac  0310a0e3  mov r1, #0x3
108b21b0  0700a0e1  cpy r0, r7
108b21b4  70dffeeb  bl 0x10869f7c
108b21b8  0050a0e1  cpy r5, r0
108b21bc  a62fa0e3  mov r2, #0x298
108b21c0  0210a0e3  mov r1, #0x2
108b21c4  040080e2  add r0, r0, #0x4
108b21c8  416cfeeb  bl 0x1084d2d4
108b21cc  0610a0e1  cpy r1, r6
108b21d0  040085e2  add r0, r5, #0x4
108b21d4  306cfeeb  bl 0x1084d29c
108b21d8  0510a0e1  cpy r1, r5
108b21dc  0700a0e1  cpy r0, r7
108b21e0  58d6feeb  bl 0x10867b48
108b21e4  0110a0e3  mov r1, #0x1
108b21e8  3f00a0e3  mov r0, #0x3f
108b21ec  b8cdfeeb  bl 0x108658d4
108b21f0  0100a0e3  mov r0, #0x1
108b21f4  180084e5  str r0, [r4,#0x18]
108b21f8  000084e5  str r0, [r4,#0x0]
108b21fc  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
