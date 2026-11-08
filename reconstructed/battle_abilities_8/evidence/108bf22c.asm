108bf22c  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108bf230  0160a0e1  cpy r6, r1
108bf234  0070a0e1  cpy r7, r0
108bf238  0250a0e1  cpy r5, r2
108bf23c  0200a0e3  mov r0, #0x2
108bf240  b5b1feeb  bl 0x1086b91c
108bf244  050050e1  cmp r0, r5
108bf248  2a00001a  bne 0x108bf2f8
108bf24c  5200a0e3  mov r0, #0x52
108bf250  b1b1feeb  bl 0x1086b91c
108bf254  000050e3  cmp r0, #0x0
108bf258  2600000a  beq 0x108bf2f8
108bf25c  2e00a0e3  mov r0, #0x2e
108bf260  adb1feeb  bl 0x1086b91c
108bf264  7000ffe6  uxth r0, r0
108bf268  4d05fdeb  bl 0x108007a4
108bf26c  000050e3  cmp r0, #0x0
108bf270  2000000a  beq 0x108bf2f8
108bf274  0510a0e1  cpy r1, r5
108bf278  0600a0e1  cpy r0, r6
108bf27c  8754feeb  bl 0x108544a0
108bf280  0080a0e1  cpy r8, r0
108bf284  0520a0e1  cpy r2, r5
108bf288  0410a0e3  mov r1, #0x4
108bf28c  0600a0e1  cpy r0, r6
108bf290  39abfeeb  bl 0x10869f7c
108bf294  0040a0e1  cpy r4, r0
108bf298  000090e5  ldr r0, [r0,#0x0]
108bf29c  0650c4e5  strb r5, [r4,#0x6]
108bf2a0  0310a0e3  mov r1, #0x3
108bf2a4  020580e3  orr r0, r0, #0x800000
108bf2a8  000084e5  str r0, [r4,#0x0]
108bf2ac  0800a0e1  cpy r0, r8
108bf2b0  0f38feeb  bl 0x1084d2f4
108bf2b4  b400c4e1  strh r0, [r4,#0x4]
108bf2b8  3c209fe5  ldr r2, [0x108bf2fc]
108bf2bc  0210a0e3  mov r1, #0x2
108bf2c0  080084e2  add r0, r4, #0x8
108bf2c4  0238feeb  bl 0x1084d2d4
108bf2c8  0510a0e1  cpy r1, r5
108bf2cc  080084e2  add r0, r4, #0x8
108bf2d0  f137feeb  bl 0x1084d29c
108bf2d4  0700a0e1  cpy r0, r7
108bf2d8  b803d0e1  ldrh r0, [r0,#0x38]
108bf2dc  0010a0e1  cpy r1, r0
108bf2e0  080084e2  add r0, r4, #0x8
108bf2e4  ec37feeb  bl 0x1084d29c
108bf2e8  0410a0e1  cpy r1, r4
108bf2ec  0600a0e1  cpy r0, r6
108bf2f0  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108bf2f4  13a2feea  b 0x10867b48
108bf2f8  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
