108b00a0  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b00a4  0240a0e1  cpy r4, r2
108b00a8  0170a0e1  cpy r7, r1
108b00ac  0300a0e3  mov r0, #0x3
108b00b0  19eefeeb  bl 0x1086b91c
108b00b4  040050e1  cmp r0, r4
108b00b8  1f00001a  bne 0x108b013c
108b00bc  0600a0e3  mov r0, #0x6
108b00c0  15eefeeb  bl 0x1086b91c
108b00c4  ff5000e2  and r5, r0, #0xff
108b00c8  0510a0e1  cpy r1, r5
108b00cc  0700a0e1  cpy r0, r7
108b00d0  f290feeb  bl 0x108544a0
108b00d4  0080a0e1  cpy r8, r0
108b00d8  0b10a0e3  mov r1, #0xb
108b00dc  699afeeb  bl 0x10856a88
108b00e0  000050e3  cmp r0, #0x0
108b00e4  1400001a  bne 0x108b013c
108b00e8  0360a0e3  mov r6, #0x3
108b00ec  0110a0e3  mov r1, #0x1
108b00f0  0800a0e1  cpy r0, r8
108b00f4  c771feeb  bl 0x1084c818
108b00f8  000050e3  cmp r0, #0x0
108b00fc  0460a013  movne r6, #0x4
108b0100  0420a0e1  cpy r2, r4
108b0104  0b10a0e3  mov r1, #0xb
108b0108  0700a0e1  cpy r0, r7
108b010c  9ae7feeb  bl 0x10869f7c
108b0110  0040a0e1  cpy r4, r0
108b0114  0b00a0e3  mov r0, #0xb
108b0118  b400c4e1  strh r0, [r4,#0x4]
108b011c  0600a0e1  cpy r0, r6
108b0120  456ffeeb  bl 0x1084be3c
108b0124  080084e5  str r0, [r4,#0x8]
108b0128  0410a0e1  cpy r1, r4
108b012c  0700a0e1  cpy r0, r7
108b0130  0f50c4e5  strb r5, [r4,#0xf]
108b0134  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108b0138  82defeea  b 0x10867b48
108b013c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
