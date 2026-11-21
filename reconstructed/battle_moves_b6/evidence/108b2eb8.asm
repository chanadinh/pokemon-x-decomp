108b2eb8  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108b2ebc  0240a0e1  cpy r4, r2
108b2ec0  0170a0e1  cpy r7, r1
108b2ec4  0300a0e3  mov r0, #0x3
108b2ec8  93e2feeb  bl 0x1086b91c
108b2ecc  040050e1  cmp r0, r4
108b2ed0  6a00001a  bne 0x108b3080
108b2ed4  0410a0e1  cpy r1, r4
108b2ed8  0700a0e1  cpy r0, r7
108b2edc  6f85feeb  bl 0x108544a0
108b2ee0  0080a0e1  cpy r8, r0
108b2ee4  0010a0e3  mov r1, #0x0
108b2ee8  0f63feeb  bl 0x1084bb2c
108b2eec  030050e3  cmp r0, #0x3
108b2ef0  0060a0e1  cpy r6, r0
108b2ef4  6100002a  bcs 0x108b3080
108b2ef8  0420a0e1  cpy r2, r4
108b2efc  2710a0e3  mov r1, #0x27
108b2f00  0700a0e1  cpy r0, r7
108b2f04  1cdcfeeb  bl 0x10869f7c
108b2f08  0010a0e1  cpy r1, r0
108b2f0c  0020a0e3  mov r2, #0x0
108b2f10  0440c1e5  strb r4, [r1,#0x4]
108b2f14  010086e2  add r0, r6, #0x1
108b2f18  0520c1e5  strb r2, [r1,#0x5]
108b2f1c  0600c1e5  strb r0, [r1,#0x6]
108b2f20  0700a0e1  cpy r0, r7
108b2f24  07d3feeb  bl 0x10867b48
108b2f28  0420a0e1  cpy r2, r4
108b2f2c  0310a0e3  mov r1, #0x3
108b2f30  0700a0e1  cpy r0, r7
108b2f34  10dcfeeb  bl 0x10869f7c
108b2f38  0050a0e1  cpy r5, r0
108b2f3c  40219fe5  ldr r2, [0x108b3084]
108b2f40  0210a0e3  mov r1, #0x2
108b2f44  040080e2  add r0, r0, #0x4
108b2f48  e168feeb  bl 0x1084d2d4
108b2f4c  0410a0e1  cpy r1, r4
108b2f50  040085e2  add r0, r5, #0x4
108b2f54  d068feeb  bl 0x1084d29c
108b2f58  011086e2  add r1, r6, #0x1
108b2f5c  040085e2  add r0, r5, #0x4
108b2f60  cd68feeb  bl 0x1084d29c
108b2f64  0510a0e1  cpy r1, r5
108b2f68  0700a0e1  cpy r0, r7
108b2f6c  f5d2feeb  bl 0x10867b48
108b2f70  0420a0e1  cpy r2, r4
108b2f74  0d10a0e3  mov r1, #0xd
108b2f78  0700a0e1  cpy r0, r7
108b2f7c  fedbfeeb  bl 0x10869f7c
108b2f80  0010a0e1  cpy r1, r0
108b2f84  1800d0e5  ldrb r0, [r0,#0x18]
108b2f88  0190a0e3  mov r9, #0x1
108b2f8c  0c40c1e5  strb r4, [r1,#0xc]
108b2f90  0260a0e3  mov r6, #0x2
108b2f94  1690c1e5  strb r9, [r1,#0x16]
108b2f98  0460c1e5  strb r6, [r1,#0x4]
108b2f9c  060080e3  orr r0, r0, #0x6
108b2fa0  1790c1e5  strb r9, [r1,#0x17]
108b2fa4  1800c1e5  strb r0, [r1,#0x18]
108b2fa8  0700a0e1  cpy r0, r7
108b2fac  e5d2feeb  bl 0x10867b48
108b2fb0  0420a0e1  cpy r2, r4
108b2fb4  2710a0e3  mov r1, #0x27
108b2fb8  0700a0e1  cpy r0, r7
108b2fbc  eedbfeeb  bl 0x10869f7c
108b2fc0  0050a0e1  cpy r5, r0
108b2fc4  000090e5  ldr r0, [r0,#0x0]
108b2fc8  0440c5e5  strb r4, [r5,#0x4]
108b2fcc  0590c5e5  strb r9, [r5,#0x5]
108b2fd0  010480e3  orr r0, r0, #0x1000000
108b2fd4  000085e5  str r0, [r5,#0x0]
108b2fd8  0110a0e3  mov r1, #0x1
108b2fdc  0800a0e1  cpy r0, r8
108b2fe0  d162feeb  bl 0x1084bb2c
108b2fe4  010080e2  add r0, r0, #0x1
108b2fe8  0600c5e5  strb r0, [r5,#0x6]
108b2fec  0510a0e1  cpy r1, r5
108b2ff0  0700a0e1  cpy r0, r7
108b2ff4  d3d2feeb  bl 0x10867b48
108b2ff8  0420a0e1  cpy r2, r4
108b2ffc  0d10a0e3  mov r1, #0xd
108b3000  0700a0e1  cpy r0, r7
108b3004  dcdbfeeb  bl 0x10869f7c
108b3008  1820d0e5  ldrb r2, [r0,#0x18]
108b300c  0010a0e1  cpy r1, r0
108b3010  0400a0e3  mov r0, #0x4
108b3014  1690c1e5  strb r9, [r1,#0x16]
108b3018  1790c1e5  strb r9, [r1,#0x17]
108b301c  0400c1e5  strb r0, [r1,#0x4]
108b3020  060082e3  orr r0, r2, #0x6
108b3024  0c40c1e5  strb r4, [r1,#0xc]
108b3028  1800c1e5  strb r0, [r1,#0x18]
108b302c  0700a0e1  cpy r0, r7
108b3030  c4d2feeb  bl 0x10867b48
108b3034  0420a0e1  cpy r2, r4
108b3038  2710a0e3  mov r1, #0x27
108b303c  0700a0e1  cpy r0, r7
108b3040  cddbfeeb  bl 0x10869f7c
108b3044  0050a0e1  cpy r5, r0
108b3048  000090e5  ldr r0, [r0,#0x0]
108b304c  0440c5e5  strb r4, [r5,#0x4]
108b3050  0560c5e5  strb r6, [r5,#0x5]
108b3054  010480e3  orr r0, r0, #0x1000000
108b3058  000085e5  str r0, [r5,#0x0]
108b305c  0210a0e3  mov r1, #0x2
108b3060  0800a0e1  cpy r0, r8
108b3064  b062feeb  bl 0x1084bb2c
108b3068  010080e2  add r0, r0, #0x1
108b306c  0600c5e5  strb r0, [r5,#0x6]
108b3070  0510a0e1  cpy r1, r5
108b3074  0700a0e1  cpy r0, r7
108b3078  f047bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108b307c  b1d2feea  b 0x10867b48
108b3080  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
