108aa970  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108aa974  0260a0e1  cpy r6, r2
108aa978  0080a0e1  cpy r8, r0
108aa97c  0170a0e1  cpy r7, r1
108aa980  0200a0e3  mov r0, #0x2
108aa984  e403ffeb  bl 0x1086b91c
108aa988  060050e1  cmp r0, r6
108aa98c  1400001a  bne 0x108aa9e4
108aa990  2f00a0e3  mov r0, #0x2f
108aa994  e003ffeb  bl 0x1086b91c
108aa998  020010e3  tst r0, #0x2
108aa99c  1000000a  beq 0x108aa9e4
108aa9a0  0700a0e1  cpy r0, r7
108aa9a4  0610a0e1  cpy r1, r6
108aa9a8  bca6feeb  bl 0x108544a0
108aa9ac  0090a0e1  cpy r9, r0
108aa9b0  5201d0e5  ldrb r0, [r0,#0x152]
108aa9b4  005050e2  subs r5, r0, #0x0
108aa9b8  0040a0e3  mov r4, #0x0
108aa9bc  0800009a  bls 0x108aa9e4
108aa9c0  ff1004e2  and r1, r4, #0xff
108aa9c4  0900a0e1  cpy r0, r9
108aa9c8  c59dfeeb  bl 0x108520e4
108aa9cc  000050e3  cmp r0, #0x0
108aa9d0  00f020e3  nop
108aa9d4  0300000a  beq 0x108aa9e8
108aa9d8  014084e2  add r4, r4, #0x1
108aa9dc  040055e1  cmp r5, r4
108aa9e0  f6ffff8a  bhi 0x108aa9c0
108aa9e4  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
108aa9e8  ff0004e2  and r0, r4, #0xff
108aa9ec  040050e3  cmp r0, #0x4
108aa9f0  fbffff0a  beq 0x108aa9e4
108aa9f4  0800a0e1  cpy r0, r8
108aa9f8  db87feeb  bl 0x1084c96c
108aa9fc  0620a0e1  cpy r2, r6
108aaa00  0700a0e1  cpy r0, r7
108aaa04  f047bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108aaa08  0010a0e3  mov r1, #0x0
108aaa0c  cd87feea  b 0x1084c948
