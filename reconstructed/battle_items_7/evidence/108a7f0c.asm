108a7f0c  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108a7f10  0260a0e1  cpy r6, r2
108a7f14  0070a0e1  cpy r7, r0
108a7f18  0180a0e1  cpy r8, r1
108a7f1c  0300a0e3  mov r0, #0x3
108a7f20  7d0effeb  bl 0x1086b91c
108a7f24  060050e1  cmp r0, r6
108a7f28  2800001a  bne 0x108a7fd0
108a7f2c  4e00a0e3  mov r0, #0x4e
108a7f30  790effeb  bl 0x1086b91c
108a7f34  000050e3  cmp r0, #0x0
108a7f38  2400001a  bne 0x108a7fd0
108a7f3c  3300a0e3  mov r0, #0x33
108a7f40  750effeb  bl 0x1086b91c
108a7f44  0040b0e1  movs r4, r0
108a7f48  2000000a  beq 0x108a7fd0
108a7f4c  0700a0e1  cpy r0, r7
108a7f50  0350a0e3  mov r5, #0x3
108a7f54  b803d0e1  ldrh r0, [r0,#0x38]
108a7f58  0510a0e1  cpy r1, r5
108a7f5c  0494feeb  bl 0x1084cf74
108a7f60  0010a0e1  cpy r1, r0
108a7f64  0400a0e1  cpy r0, r4
108a7f68  2b61fdeb  bl 0x1080041c
108a7f6c  0050b0e1  movs r5, r0
108a7f70  0150a003  moveq r5, #0x1
108a7f74  0620a0e1  cpy r2, r6
108a7f78  0410a0e3  mov r1, #0x4
108a7f7c  0800a0e1  cpy r0, r8
108a7f80  fd07ffeb  bl 0x10869f7c
108a7f84  0660c0e5  strb r6, [r0,#0x6]
108a7f88  0040a0e1  cpy r4, r0
108a7f8c  b450c0e1  strh r5, [r0,#0x4]
108a7f90  e62fa0e3  mov r2, #0x398
108a7f94  0210a0e3  mov r1, #0x2
108a7f98  080080e2  add r0, r0, #0x8
108a7f9c  cc94feeb  bl 0x1084d2d4
108a7fa0  0610a0e1  cpy r1, r6
108a7fa4  080084e2  add r0, r4, #0x8
108a7fa8  bb94feeb  bl 0x1084d29c
108a7fac  0700a0e1  cpy r0, r7
108a7fb0  b803d0e1  ldrh r0, [r0,#0x38]
108a7fb4  0010a0e1  cpy r1, r0
108a7fb8  080084e2  add r0, r4, #0x8
108a7fbc  b694feeb  bl 0x1084d29c
108a7fc0  0410a0e1  cpy r1, r4
108a7fc4  0800a0e1  cpy r0, r8
108a7fc8  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108a7fcc  ddfefeea  b 0x10867b48
108a7fd0  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
