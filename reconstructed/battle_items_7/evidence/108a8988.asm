108a8988  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108a898c  0160a0e1  cpy r6, r1
108a8990  0070a0e1  cpy r7, r0
108a8994  0250a0e1  cpy r5, r2
108a8998  0200a0e3  mov r0, #0x2
108a899c  de0bffeb  bl 0x1086b91c
108a89a0  050050e1  cmp r0, r5
108a89a4  2000001a  bne 0x108a8a2c
108a89a8  0510a0e1  cpy r1, r5
108a89ac  0600a0e1  cpy r0, r6
108a89b0  baaefeeb  bl 0x108544a0
108a89b4  0080a0e1  cpy r8, r0
108a89b8  5692feeb  bl 0x1084d318
108a89bc  000050e3  cmp r0, #0x0
108a89c0  1900001a  bne 0x108a8a2c
108a89c4  0520a0e1  cpy r2, r5
108a89c8  0410a0e3  mov r1, #0x4
108a89cc  0600a0e1  cpy r0, r6
108a89d0  6905ffeb  bl 0x10869f7c
108a89d4  0040a0e1  cpy r4, r0
108a89d8  0650c0e5  strb r5, [r0,#0x6]
108a89dc  1010a0e3  mov r1, #0x10
108a89e0  0800a0e1  cpy r0, r8
108a89e4  4292feeb  bl 0x1084d2f4
108a89e8  b400c4e1  strh r0, [r4,#0x4]
108a89ec  e62fa0e3  mov r2, #0x398
108a89f0  0210a0e3  mov r1, #0x2
108a89f4  080084e2  add r0, r4, #0x8
108a89f8  3592feeb  bl 0x1084d2d4
108a89fc  0510a0e1  cpy r1, r5
108a8a00  080084e2  add r0, r4, #0x8
108a8a04  2492feeb  bl 0x1084d29c
108a8a08  0700a0e1  cpy r0, r7
108a8a0c  b803d0e1  ldrh r0, [r0,#0x38]
108a8a10  0010a0e1  cpy r1, r0
108a8a14  080084e2  add r0, r4, #0x8
108a8a18  1f92feeb  bl 0x1084d29c
108a8a1c  0410a0e1  cpy r1, r4
108a8a20  0600a0e1  cpy r0, r6
108a8a24  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108a8a28  46fcfeea  b 0x10867b48
108a8a2c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
