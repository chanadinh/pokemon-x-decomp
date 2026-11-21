108b3708  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108b370c  0250a0e1  cpy r5, r2
108b3710  0180a0e1  cpy r8, r1
108b3714  0300a0e3  mov r0, #0x3
108b3718  7fe0feeb  bl 0x1086b91c
108b371c  050050e1  cmp r0, r5
108b3720  4100001a  bne 0x108b382c
108b3724  0600a0e3  mov r0, #0x6
108b3728  7be0feeb  bl 0x1086b91c
108b372c  ff4000e2  and r4, r0, #0xff
108b3730  0510a0e1  cpy r1, r5
108b3734  0800a0e1  cpy r0, r8
108b3738  5883feeb  bl 0x108544a0
108b373c  0060a0e1  cpy r6, r0
108b3740  0410a0e1  cpy r1, r4
108b3744  0800a0e1  cpy r0, r8
108b3748  5483feeb  bl 0x108544a0
108b374c  0090a0e1  cpy r9, r0
108b3750  0910a0e3  mov r1, #0x9
108b3754  0600a0e1  cpy r0, r6
108b3758  1203ffeb  bl 0x108743a8
108b375c  0070a0e1  cpy r7, r0
108b3760  0910a0e3  mov r1, #0x9
108b3764  0900a0e1  cpy r0, r9
108b3768  0e03ffeb  bl 0x108743a8
108b376c  070080e0  add r0, r0, r7
108b3770  0b10a0e3  mov r1, #0xb
108b3774  a00f80e0  add r0, r0, r0, lsr #0x1f
108b3778  c070a0e1  mov r7, r0, asr #0x1
108b377c  0600a0e1  cpy r0, r6
108b3780  0803ffeb  bl 0x108743a8
108b3784  0060a0e1  cpy r6, r0
108b3788  0b10a0e3  mov r1, #0xb
108b378c  0900a0e1  cpy r0, r9
108b3790  0403ffeb  bl 0x108743a8
108b3794  060080e0  add r0, r0, r6
108b3798  0520a0e1  cpy r2, r5
108b379c  a00f80e0  add r0, r0, r0, lsr #0x1f
108b37a0  1010a0e3  mov r1, #0x10
108b37a4  c060a0e1  mov r6, r0, asr #0x1
108b37a8  0800a0e1  cpy r0, r8
108b37ac  f2d9feeb  bl 0x10869f7c
108b37b0  0f20d0e5  ldrb r2, [r0,#0xf]
108b37b4  b670c0e1  strh r7, [r0,#0x6]
108b37b8  0e40c0e5  strb r4, [r0,#0xe]
108b37bc  0010a0e1  cpy r1, r0
108b37c0  ba60c0e1  strh r6, [r0,#0xa]
108b37c4  0a0082e3  orr r0, r2, #0xa
108b37c8  0f00c1e5  strb r0, [r1,#0xf]
108b37cc  0800a0e1  cpy r0, r8
108b37d0  dcd0feeb  bl 0x10867b48
108b37d4  0520a0e1  cpy r2, r5
108b37d8  1010a0e3  mov r1, #0x10
108b37dc  0800a0e1  cpy r0, r8
108b37e0  e5d9feeb  bl 0x10869f7c
108b37e4  0040a0e1  cpy r4, r0
108b37e8  0f00d0e5  ldrb r0, [r0,#0xf]
108b37ec  b670c4e1  strh r7, [r4,#0x6]
108b37f0  ba60c4e1  strh r6, [r4,#0xa]
108b37f4  0a0080e3  orr r0, r0, #0xa
108b37f8  0e50c4e5  strb r5, [r4,#0xe]
108b37fc  0f00c4e5  strb r0, [r4,#0xf]
108b3800  28209fe5  ldr r2, [0x108b3830]
108b3804  0210a0e3  mov r1, #0x2
108b3808  100084e2  add r0, r4, #0x10
108b380c  b066feeb  bl 0x1084d2d4
108b3810  0510a0e1  cpy r1, r5
108b3814  100084e2  add r0, r4, #0x10
108b3818  9f66feeb  bl 0x1084d29c
108b381c  0410a0e1  cpy r1, r4
108b3820  0800a0e1  cpy r0, r8
108b3824  f047bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108b3828  c6d0feea  b 0x10867b48
108b382c  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
