108ab898  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108ab89c  0250a0e1  cpy r5, r2
108ab8a0  0170a0e1  cpy r7, r1
108ab8a4  0300a0e3  mov r0, #0x3
108ab8a8  1b00ffeb  bl 0x1086b91c
108ab8ac  050050e1  cmp r0, r5
108ab8b0  1500001a  bne 0x108ab90c
108ab8b4  1200a0e3  mov r0, #0x12
108ab8b8  1700ffeb  bl 0x1086b91c
108ab8bc  7060ffe6  uxth r6, r0
108ab8c0  a50056e3  cmp r6, #0xa5
108ab8c4  1000000a  beq 0x108ab90c
108ab8c8  0520a0e1  cpy r2, r5
108ab8cc  0b10a0e3  mov r1, #0xb
108ab8d0  0700a0e1  cpy r0, r7
108ab8d4  a8f9feeb  bl 0x10869f7c
108ab8d8  0040a0e1  cpy r4, r0
108ab8dc  1b00a0e3  mov r0, #0x1b
108ab8e0  0f50c4e5  strb r5, [r4,#0xf]
108ab8e4  b400c4e1  strh r0, [r4,#0x4]
108ab8e8  0600a0e1  cpy r0, r6
108ab8ec  af82feeb  bl 0x1084c3b0
108ab8f0  080084e5  str r0, [r4,#0x8]
108ab8f4  0210a0e3  mov r1, #0x2
108ab8f8  1010c4e5  strb r1, [r4,#0x10]
108ab8fc  0410a0e1  cpy r1, r4
108ab900  0700a0e1  cpy r0, r7
108ab904  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108ab908  8ef0feea  b 0x10867b48
108ab90c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
