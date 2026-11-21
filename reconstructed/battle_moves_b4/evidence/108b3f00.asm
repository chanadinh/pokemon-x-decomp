108b3f00  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108b3f04  0250a0e1  cpy r5, r2
108b3f08  0180a0e1  cpy r8, r1
108b3f0c  0300a0e3  mov r0, #0x3
108b3f10  81defeeb  bl 0x1086b91c
108b3f14  050050e1  cmp r0, r5
108b3f18  4100001a  bne 0x108b4024
108b3f1c  0600a0e3  mov r0, #0x6
108b3f20  7ddefeeb  bl 0x1086b91c
108b3f24  ff4000e2  and r4, r0, #0xff
108b3f28  0510a0e1  cpy r1, r5
108b3f2c  0800a0e1  cpy r0, r8
108b3f30  5a81feeb  bl 0x108544a0
108b3f34  0060a0e1  cpy r6, r0
108b3f38  0410a0e1  cpy r1, r4
108b3f3c  0800a0e1  cpy r0, r8
108b3f40  5681feeb  bl 0x108544a0
108b3f44  0090a0e1  cpy r9, r0
108b3f48  0810a0e3  mov r1, #0x8
108b3f4c  0600a0e1  cpy r0, r6
108b3f50  1401ffeb  bl 0x108743a8
108b3f54  0070a0e1  cpy r7, r0
108b3f58  0810a0e3  mov r1, #0x8
108b3f5c  0900a0e1  cpy r0, r9
108b3f60  1001ffeb  bl 0x108743a8
108b3f64  070080e0  add r0, r0, r7
108b3f68  0a10a0e3  mov r1, #0xa
108b3f6c  a00f80e0  add r0, r0, r0, lsr #0x1f
108b3f70  c070a0e1  mov r7, r0, asr #0x1
108b3f74  0600a0e1  cpy r0, r6
108b3f78  0a01ffeb  bl 0x108743a8
108b3f7c  0060a0e1  cpy r6, r0
108b3f80  0a10a0e3  mov r1, #0xa
108b3f84  0900a0e1  cpy r0, r9
108b3f88  0601ffeb  bl 0x108743a8
108b3f8c  060080e0  add r0, r0, r6
108b3f90  0520a0e1  cpy r2, r5
108b3f94  a00f80e0  add r0, r0, r0, lsr #0x1f
108b3f98  1010a0e3  mov r1, #0x10
108b3f9c  c060a0e1  mov r6, r0, asr #0x1
108b3fa0  0800a0e1  cpy r0, r8
108b3fa4  f4d7feeb  bl 0x10869f7c
108b3fa8  0f20d0e5  ldrb r2, [r0,#0xf]
108b3fac  b470c0e1  strh r7, [r0,#0x4]
108b3fb0  0e40c0e5  strb r4, [r0,#0xe]
108b3fb4  0010a0e1  cpy r1, r0
108b3fb8  b860c0e1  strh r6, [r0,#0x8]
108b3fbc  050082e3  orr r0, r2, #0x5
108b3fc0  0f00c1e5  strb r0, [r1,#0xf]
108b3fc4  0800a0e1  cpy r0, r8
108b3fc8  decefeeb  bl 0x10867b48
108b3fcc  0520a0e1  cpy r2, r5
108b3fd0  1010a0e3  mov r1, #0x10
108b3fd4  0800a0e1  cpy r0, r8
108b3fd8  e7d7feeb  bl 0x10869f7c
108b3fdc  0040a0e1  cpy r4, r0
108b3fe0  0f00d0e5  ldrb r0, [r0,#0xf]
108b3fe4  b470c4e1  strh r7, [r4,#0x4]
108b3fe8  b860c4e1  strh r6, [r4,#0x8]
108b3fec  050080e3  orr r0, r0, #0x5
108b3ff0  0e50c4e5  strb r5, [r4,#0xe]
108b3ff4  0f00c4e5  strb r0, [r4,#0xf]
108b3ff8  28209fe5  ldr r2, [0x108b4028]
108b3ffc  0210a0e3  mov r1, #0x2
108b4000  100084e2  add r0, r4, #0x10
108b4004  b264feeb  bl 0x1084d2d4
108b4008  0510a0e1  cpy r1, r5
108b400c  100084e2  add r0, r4, #0x10
108b4010  a164feeb  bl 0x1084d29c
108b4014  0410a0e1  cpy r1, r4
108b4018  0800a0e1  cpy r0, r8
108b401c  f047bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108b4020  c8cefeea  b 0x10867b48
108b4024  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
