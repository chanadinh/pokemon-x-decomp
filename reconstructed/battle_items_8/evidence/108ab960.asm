108ab960  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108ab964  0160a0e1  cpy r6, r1
108ab968  0070a0e1  cpy r7, r0
108ab96c  000093e5  ldr r0, [r3,#0x0]
108ab970  0250a0e1  cpy r5, r2
108ab974  0340a0e1  cpy r4, r3
108ab978  000050e3  cmp r0, #0x0
108ab97c  1f0000da  ble 0x108aba00
108ab980  0400a0e3  mov r0, #0x4
108ab984  e4fffeeb  bl 0x1086b91c
108ab988  050050e1  cmp r0, r5
108ab98c  1b00001a  bne 0x108aba00
108ab990  0000a0e3  mov r0, #0x0
108ab994  000084e5  str r0, [r4,#0x0]
108ab998  0510a0e1  cpy r1, r5
108ab99c  0600a0e1  cpy r0, r6
108ab9a0  5ed7fdeb  bl 0x10821720
108ab9a4  0520a0e1  cpy r2, r5
108ab9a8  2410a0e3  mov r1, #0x24
108ab9ac  0600a0e1  cpy r0, r6
108ab9b0  71f9feeb  bl 0x10869f7c
108ab9b4  0040a0e1  cpy r4, r0
108ab9b8  0100a0e3  mov r0, #0x1
108ab9bc  0400c4e5  strb r0, [r4,#0x4]
108ab9c0  db20a0e3  mov r2, #0xdb
108ab9c4  0210a0e3  mov r1, #0x2
108ab9c8  080084e2  add r0, r4, #0x8
108ab9cc  4086feeb  bl 0x1084d2d4
108ab9d0  0510a0e1  cpy r1, r5
108ab9d4  080084e2  add r0, r4, #0x8
108ab9d8  2f86feeb  bl 0x1084d29c
108ab9dc  0700a0e1  cpy r0, r7
108ab9e0  b803d0e1  ldrh r0, [r0,#0x38]
108ab9e4  0010a0e1  cpy r1, r0
108ab9e8  080084e2  add r0, r4, #0x8
108ab9ec  2a86feeb  bl 0x1084d29c
108ab9f0  0410a0e1  cpy r1, r4
108ab9f4  0600a0e1  cpy r0, r6
108ab9f8  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108ab9fc  51f0feea  b 0x10867b48
108aba00  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
