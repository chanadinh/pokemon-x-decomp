108a6e58  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108a6e5c  0050a0e1  cpy r5, r0
108a6e60  0170a0e1  cpy r7, r1
108a6e64  0240a0e1  cpy r4, r2
108a6e68  0400a0e3  mov r0, #0x4
108a6e6c  aa12ffeb  bl 0x1086b91c
108a6e70  040050e1  cmp r0, r4
108a6e74  2a00001a  bne 0x108a6f24
108a6e78  4700a0e3  mov r0, #0x47
108a6e7c  a612ffeb  bl 0x1086b91c
108a6e80  000050e3  cmp r0, #0x0
108a6e84  2600001a  bne 0x108a6f24
108a6e88  1200a0e3  mov r0, #0x12
108a6e8c  a212ffeb  bl 0x1086b91c
108a6e90  7000ffe6  uxth r0, r0
108a6e94  0010a0e3  mov r1, #0x0
108a6e98  7f65fdeb  bl 0x1080049c
108a6e9c  000050e3  cmp r0, #0x0
108a6ea0  1f00000a  beq 0x108a6f24
108a6ea4  0420a0e1  cpy r2, r4
108a6ea8  0610a0e3  mov r1, #0x6
108a6eac  0700a0e1  cpy r0, r7
108a6eb0  310cffeb  bl 0x10869f7c
108a6eb4  0040a0e1  cpy r4, r0
108a6eb8  0300a0e3  mov r0, #0x3
108a6ebc  9612ffeb  bl 0x1086b91c
108a6ec0  ff1000e2  and r1, r0, #0xff
108a6ec4  0700a0e1  cpy r0, r7
108a6ec8  0610c4e5  strb r1, [r4,#0x6]
108a6ecc  73b5feeb  bl 0x108544a0
108a6ed0  0080a0e1  cpy r8, r0
108a6ed4  0500a0e1  cpy r0, r5
108a6ed8  0360a0e3  mov r6, #0x3
108a6edc  b803d0e1  ldrh r0, [r0,#0x38]
108a6ee0  0610a0e1  cpy r1, r6
108a6ee4  2298feeb  bl 0x1084cf74
108a6ee8  ff1000e2  and r1, r0, #0xff
108a6eec  0800a0e1  cpy r0, r8
108a6ef0  ff98feeb  bl 0x1084d2f4
108a6ef4  b400c4e1  strh r0, [r4,#0x4]
108a6ef8  6a2fa0e3  mov r2, #0x1a8
108a6efc  0210a0e3  mov r1, #0x2
108a6f00  0c0084e2  add r0, r4, #0xc
108a6f04  f298feeb  bl 0x1084d2d4
108a6f08  0610d4e5  ldrb r1, [r4,#0x6]
108a6f0c  0c0084e2  add r0, r4, #0xc
108a6f10  e198feeb  bl 0x1084d29c
108a6f14  0410a0e1  cpy r1, r4
108a6f18  0700a0e1  cpy r0, r7
108a6f1c  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108a6f20  0803ffea  b 0x10867b48
108a6f24  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
