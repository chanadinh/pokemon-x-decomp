108b6af0  f05f2de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108b6af4  0240a0e1  cpy r4, r2
108b6af8  0180a0e1  cpy r8, r1
108b6afc  0200a0e3  mov r0, #0x2
108b6b00  85d3feeb  bl 0x1086b91c
108b6b04  040050e1  cmp r0, r4
108b6b08  6400001a  bne 0x108b6ca0
108b6b0c  0410a0e1  cpy r1, r4
108b6b10  0800a0e1  cpy r0, r8
108b6b14  6176feeb  bl 0x108544a0
108b6b18  0070a0e1  cpy r7, r0
108b6b1c  0110a0e3  mov r1, #0x1
108b6b20  0154feeb  bl 0x1084bb2c
108b6b24  0050a0e1  cpy r5, r0
108b6b28  0210a0e3  mov r1, #0x2
108b6b2c  0700a0e1  cpy r0, r7
108b6b30  fd53feeb  bl 0x1084bb2c
108b6b34  0060a0e1  cpy r6, r0
108b6b38  0010a0e3  mov r1, #0x0
108b6b3c  0700a0e1  cpy r0, r7
108b6b40  f953feeb  bl 0x1084bb2c
108b6b44  000055e3  cmp r5, #0x0
108b6b48  0070a0e1  cpy r7, r0
108b6b4c  02b0a0e3  mov r11, #0x2
108b6b50  01a0a0e3  mov r10, #0x1
108b6b54  0090a0e3  mov r9, #0x0
108b6b58  1800000a  beq 0x108b6bc0
108b6b5c  0420a0e1  cpy r2, r4
108b6b60  0d10a0e3  mov r1, #0xd
108b6b64  0800a0e1  cpy r0, r8
108b6b68  03cdfeeb  bl 0x10869f7c
108b6b6c  0010a0e1  cpy r1, r0
108b6b70  1800d0e5  ldrb r0, [r0,#0x18]
108b6b74  04b0c1e5  strb r11, [r1,#0x4]
108b6b78  002065e2  rsb r2, r5, #0x0
108b6b7c  16a0c1e5  strb r10, [r1,#0x16]
108b6b80  040080e3  orr r0, r0, #0x4
108b6b84  1720c1e5  strb r2, [r1,#0x17]
108b6b88  1800c1e5  strb r0, [r1,#0x18]
108b6b8c  0800a0e1  cpy r0, r8
108b6b90  0c40c1e5  strb r4, [r1,#0xc]
108b6b94  ebc3feeb  bl 0x10867b48
108b6b98  0420a0e1  cpy r2, r4
108b6b9c  2710a0e3  mov r1, #0x27
108b6ba0  0800a0e1  cpy r0, r8
108b6ba4  f4ccfeeb  bl 0x10869f7c
108b6ba8  05a0c0e5  strb r10, [r0,#0x5]
108b6bac  0440c0e5  strb r4, [r0,#0x4]
108b6bb0  0010a0e1  cpy r1, r0
108b6bb4  0690c0e5  strb r9, [r0,#0x6]
108b6bb8  0800a0e1  cpy r0, r8
108b6bbc  e1c3feeb  bl 0x10867b48
108b6bc0  000056e3  cmp r6, #0x0
108b6bc4  1900000a  beq 0x108b6c30
108b6bc8  0420a0e1  cpy r2, r4
108b6bcc  0d10a0e3  mov r1, #0xd
108b6bd0  0800a0e1  cpy r0, r8
108b6bd4  e8ccfeeb  bl 0x10869f7c
108b6bd8  1820d0e5  ldrb r2, [r0,#0x18]
108b6bdc  0010a0e1  cpy r1, r0
108b6be0  0430a0e3  mov r3, #0x4
108b6be4  16a0c1e5  strb r10, [r1,#0x16]
108b6be8  000066e2  rsb r0, r6, #0x0
108b6bec  0430c1e5  strb r3, [r1,#0x4]
108b6bf0  1700c1e5  strb r0, [r1,#0x17]
108b6bf4  042082e3  orr r2, r2, #0x4
108b6bf8  1820c1e5  strb r2, [r1,#0x18]
108b6bfc  0800a0e1  cpy r0, r8
108b6c00  0c40c1e5  strb r4, [r1,#0xc]
108b6c04  cfc3feeb  bl 0x10867b48
108b6c08  0420a0e1  cpy r2, r4
108b6c0c  2710a0e3  mov r1, #0x27
108b6c10  0800a0e1  cpy r0, r8
108b6c14  d8ccfeeb  bl 0x10869f7c
108b6c18  0440c0e5  strb r4, [r0,#0x4]
108b6c1c  05b0c0e5  strb r11, [r0,#0x5]
108b6c20  0010a0e1  cpy r1, r0
108b6c24  0690c0e5  strb r9, [r0,#0x6]
108b6c28  0800a0e1  cpy r0, r8
108b6c2c  c5c3feeb  bl 0x10867b48
108b6c30  000057e3  cmp r7, #0x0
108b6c34  1900000a  beq 0x108b6ca0
108b6c38  0420a0e1  cpy r2, r4
108b6c3c  2710a0e3  mov r1, #0x27
108b6c40  0800a0e1  cpy r0, r8
108b6c44  ccccfeeb  bl 0x10869f7c
108b6c48  0440c0e5  strb r4, [r0,#0x4]
108b6c4c  0590c0e5  strb r9, [r0,#0x5]
108b6c50  0010a0e1  cpy r1, r0
108b6c54  0690c0e5  strb r9, [r0,#0x6]
108b6c58  0800a0e1  cpy r0, r8
108b6c5c  b9c3feeb  bl 0x10867b48
108b6c60  0420a0e1  cpy r2, r4
108b6c64  0310a0e3  mov r1, #0x3
108b6c68  0800a0e1  cpy r0, r8
108b6c6c  c2ccfeeb  bl 0x10869f7c
108b6c70  0050a0e1  cpy r5, r0
108b6c74  b52fa0e3  mov r2, #0x2d4
108b6c78  0210a0e3  mov r1, #0x2
108b6c7c  040080e2  add r0, r0, #0x4
108b6c80  9359feeb  bl 0x1084d2d4
108b6c84  0410a0e1  cpy r1, r4
108b6c88  040085e2  add r0, r5, #0x4
108b6c8c  8259feeb  bl 0x1084d29c
108b6c90  0510a0e1  cpy r1, r5
108b6c94  0800a0e1  cpy r0, r8
108b6c98  f05fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108b6c9c  a9c3feea  b 0x10867b48
108b6ca0  f09fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,pc}
