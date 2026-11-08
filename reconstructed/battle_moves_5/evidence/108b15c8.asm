108b15c8  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b15cc  0350a0e3  mov r5, #0x3
108b15d0  0160a0e1  cpy r6, r1
108b15d4  0240a0e1  cpy r4, r2
108b15d8  d170a0e3  mov r7, #0xd1
108b15dc  0500a0e1  cpy r0, r5
108b15e0  cde8feeb  bl 0x1086b91c
108b15e4  040050e1  cmp r0, r4
108b15e8  1600001a  bne 0x108b1648
108b15ec  0600a0e1  cpy r0, r6
108b15f0  b768feeb  bl 0x1084b8d4
108b15f4  030050e3  cmp r0, #0x3
108b15f8  1200000a  beq 0x108b1648
108b15fc  0420a0e1  cpy r2, r4
108b1600  1c10a0e3  mov r1, #0x1c
108b1604  0600a0e1  cpy r0, r6
108b1608  5be2feeb  bl 0x10869f7c
108b160c  0040a0e1  cpy r4, r0
108b1610  0a00a0e3  mov r0, #0xa
108b1614  0550c4e5  strb r5, [r4,#0x5]
108b1618  0400c4e5  strb r0, [r4,#0x4]
108b161c  0500a0e3  mov r0, #0x5
108b1620  056afeeb  bl 0x1084be3c
108b1624  080084e5  str r0, [r4,#0x8]
108b1628  0720a0e1  cpy r2, r7
108b162c  0110a0e3  mov r1, #0x1
108b1630  0c0084e2  add r0, r4, #0xc
108b1634  266ffeeb  bl 0x1084d2d4
108b1638  0410a0e1  cpy r1, r4
108b163c  0600a0e1  cpy r0, r6
108b1640  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108b1644  3fd9feea  b 0x10867b48
108b1648  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
