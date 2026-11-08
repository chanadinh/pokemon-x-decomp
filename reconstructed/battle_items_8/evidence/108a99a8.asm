108a99a8  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108a99ac  0060a0e1  cpy r6, r0
108a99b0  0350a0e1  cpy r5, r3
108a99b4  0170a0e1  cpy r7, r1
108a99b8  0240a0e1  cpy r4, r2
108a99bc  0400a0e3  mov r0, #0x4
108a99c0  d507ffeb  bl 0x1086b91c
108a99c4  040050e1  cmp r0, r4
108a99c8  1e00001a  bne 0x108a9a48
108a99cc  4700a0e3  mov r0, #0x47
108a99d0  d107ffeb  bl 0x1086b91c
108a99d4  000050e3  cmp r0, #0x0
108a99d8  1a00001a  bne 0x108a9a48
108a99dc  1200a0e3  mov r0, #0x12
108a99e0  cd07ffeb  bl 0x1086b91c
108a99e4  7000ffe6  uxth r0, r0
108a99e8  868bfeeb  bl 0x1084c808
108a99ec  000050e3  cmp r0, #0x0
108a99f0  1400001a  bne 0x108a9a48
108a99f4  1b00a0e3  mov r0, #0x1b
108a99f8  c707ffeb  bl 0x1086b91c
108a99fc  020050e3  cmp r0, #0x2
108a9a00  1000001a  bne 0x108a9a48
108a9a04  0600a0e1  cpy r0, r6
108a9a08  d78bfeeb  bl 0x1084c96c
108a9a0c  0300a0e3  mov r0, #0x3
108a9a10  c107ffeb  bl 0x1086b91c
108a9a14  010080e2  add r0, r0, #0x1
108a9a18  000085e5  str r0, [r5,#0x0]
108a9a1c  0420a0e1  cpy r2, r4
108a9a20  0010a0e3  mov r1, #0x0
108a9a24  0700a0e1  cpy r0, r7
108a9a28  5301ffeb  bl 0x10869f7c
108a9a2c  0010a0e1  cpy r1, r0
108a9a30  040090e5  ldr r0, [r0,#0x4]
108a9a34  020080e3  orr r0, r0, #0x2
108a9a38  040081e5  str r0, [r1,#0x4]
108a9a3c  0700a0e1  cpy r0, r7
108a9a40  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108a9a44  3ff8feea  b 0x10867b48
108a9a48  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
