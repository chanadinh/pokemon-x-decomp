108b402c  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b4030  0160a0e1  cpy r6, r1
108b4034  0250a0e1  cpy r5, r2
108b4038  0300a0e3  mov r0, #0x3
108b403c  36defeeb  bl 0x1086b91c
108b4040  050050e1  cmp r0, r5
108b4044  2a00001a  bne 0x108b40f4
108b4048  0510a0e1  cpy r1, r5
108b404c  0600a0e1  cpy r0, r6
108b4050  1281feeb  bl 0x108544a0
108b4054  0070a0e1  cpy r7, r0
108b4058  0520a0e1  cpy r2, r5
108b405c  1010a0e3  mov r1, #0x10
108b4060  0600a0e1  cpy r0, r6
108b4064  c4d7feeb  bl 0x10869f7c
108b4068  0040a0e1  cpy r4, r0
108b406c  0e50c0e5  strb r5, [r0,#0xe]
108b4070  0910a0e3  mov r1, #0x9
108b4074  0700a0e1  cpy r0, r7
108b4078  ca00ffeb  bl 0x108743a8
108b407c  b400c4e1  strh r0, [r4,#0x4]
108b4080  0810a0e3  mov r1, #0x8
108b4084  0700a0e1  cpy r0, r7
108b4088  c600ffeb  bl 0x108743a8
108b408c  0020a0e1  cpy r2, r0
108b4090  0f00d4e5  ldrb r0, [r4,#0xf]
108b4094  b620c4e1  strh r2, [r4,#0x6]
108b4098  58209fe5  ldr r2, [0x108b40f8]
108b409c  030080e3  orr r0, r0, #0x3
108b40a0  0f00c4e5  strb r0, [r4,#0xf]
108b40a4  0210a0e3  mov r1, #0x2
108b40a8  100084e2  add r0, r4, #0x10
108b40ac  8864feeb  bl 0x1084d2d4
108b40b0  0510a0e1  cpy r1, r5
108b40b4  100084e2  add r0, r4, #0x10
108b40b8  7764feeb  bl 0x1084d29c
108b40bc  0410a0e1  cpy r1, r4
108b40c0  0600a0e1  cpy r0, r6
108b40c4  9fcefeeb  bl 0x10867b48
108b40c8  0520a0e1  cpy r2, r5
108b40cc  1710a0e3  mov r1, #0x17
108b40d0  0600a0e1  cpy r0, r6
108b40d4  a8d7feeb  bl 0x10869f7c
108b40d8  0010a0e1  cpy r1, r0
108b40dc  0a00a0e3  mov r0, #0xa
108b40e0  0550c1e5  strb r5, [r1,#0x5]
108b40e4  0400c1e5  strb r0, [r1,#0x4]
108b40e8  0600a0e1  cpy r0, r6
108b40ec  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108b40f0  94cefeea  b 0x10867b48
108b40f4  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
