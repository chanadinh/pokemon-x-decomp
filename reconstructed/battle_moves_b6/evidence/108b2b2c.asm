108b2b2c  f05f2de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108b2b30  0250a0e1  cpy r5, r2
108b2b34  0300a0e3  mov r0, #0x3
108b2b38  0190a0e1  cpy r9, r1
108b2b3c  76e3feeb  bl 0x1086b91c
108b2b40  050050e1  cmp r0, r5
108b2b44  6b00001a  bne 0x108b2cf8
108b2b48  0600a0e3  mov r0, #0x6
108b2b4c  72e3feeb  bl 0x1086b91c
108b2b50  ff6000e2  and r6, r0, #0xff
108b2b54  0610a0e1  cpy r1, r6
108b2b58  0900a0e1  cpy r0, r9
108b2b5c  4f86feeb  bl 0x108544a0
108b2b60  00b0a0e1  cpy r11, r0
108b2b64  0510a0e1  cpy r1, r5
108b2b68  0900a0e1  cpy r0, r9
108b2b6c  4b86feeb  bl 0x108544a0
108b2b70  00a0a0e1  cpy r10, r0
108b2b74  0110a0e3  mov r1, #0x1
108b2b78  2806ffeb  bl 0x10874420
108b2b7c  0070a0e1  cpy r7, r0
108b2b80  0310a0e3  mov r1, #0x3
108b2b84  0a00a0e1  cpy r0, r10
108b2b88  2406ffeb  bl 0x10874420
108b2b8c  0080a0e1  cpy r8, r0
108b2b90  0520a0e1  cpy r2, r5
108b2b94  0e10a0e3  mov r1, #0xe
108b2b98  0900a0e1  cpy r0, r9
108b2b9c  f6dcfeeb  bl 0x10869f7c
108b2ba0  0040a0e1  cpy r4, r0
108b2ba4  0450c0e5  strb r5, [r0,#0x4]
108b2ba8  0110a0e3  mov r1, #0x1
108b2bac  0b00a0e1  cpy r0, r11
108b2bb0  1a06ffeb  bl 0x10874420
108b2bb4  0500c4e5  strb r0, [r4,#0x5]
108b2bb8  0310a0e3  mov r1, #0x3
108b2bbc  0b00a0e1  cpy r0, r11
108b2bc0  1606ffeb  bl 0x10874420
108b2bc4  0700c4e5  strb r0, [r4,#0x7]
108b2bc8  0210a0e3  mov r1, #0x2
108b2bcc  0a00a0e1  cpy r0, r10
108b2bd0  1206ffeb  bl 0x10874420
108b2bd4  0600c4e5  strb r0, [r4,#0x6]
108b2bd8  0410a0e3  mov r1, #0x4
108b2bdc  0a00a0e1  cpy r0, r10
108b2be0  0e06ffeb  bl 0x10874420
108b2be4  0800c4e5  strb r0, [r4,#0x8]
108b2be8  0510a0e3  mov r1, #0x5
108b2bec  0a00a0e1  cpy r0, r10
108b2bf0  0a06ffeb  bl 0x10874420
108b2bf4  0900c4e5  strb r0, [r4,#0x9]
108b2bf8  0610a0e3  mov r1, #0x6
108b2bfc  0a00a0e1  cpy r0, r10
108b2c00  0606ffeb  bl 0x10874420
108b2c04  0a00c4e5  strb r0, [r4,#0xa]
108b2c08  0710a0e3  mov r1, #0x7
108b2c0c  0a00a0e1  cpy r0, r10
108b2c10  0206ffeb  bl 0x10874420
108b2c14  0b00c4e5  strb r0, [r4,#0xb]
108b2c18  0a00a0e1  cpy r0, r10
108b2c1c  5501d0e5  ldrb r0, [r0,#0x155]
108b2c20  0c00c4e5  strb r0, [r4,#0xc]
108b2c24  0410a0e1  cpy r1, r4
108b2c28  0900a0e1  cpy r0, r9
108b2c2c  c5d3feeb  bl 0x10867b48
108b2c30  0520a0e1  cpy r2, r5
108b2c34  0e10a0e3  mov r1, #0xe
108b2c38  0900a0e1  cpy r0, r9
108b2c3c  cedcfeeb  bl 0x10869f7c
108b2c40  0570c0e5  strb r7, [r0,#0x5]
108b2c44  0460c0e5  strb r6, [r0,#0x4]
108b2c48  0040a0e1  cpy r4, r0
108b2c4c  0780c0e5  strb r8, [r0,#0x7]
108b2c50  0210a0e3  mov r1, #0x2
108b2c54  0b00a0e1  cpy r0, r11
108b2c58  f005ffeb  bl 0x10874420
108b2c5c  0600c4e5  strb r0, [r4,#0x6]
108b2c60  0410a0e3  mov r1, #0x4
108b2c64  0b00a0e1  cpy r0, r11
108b2c68  ec05ffeb  bl 0x10874420
108b2c6c  0800c4e5  strb r0, [r4,#0x8]
108b2c70  0510a0e3  mov r1, #0x5
108b2c74  0b00a0e1  cpy r0, r11
108b2c78  e805ffeb  bl 0x10874420
108b2c7c  0900c4e5  strb r0, [r4,#0x9]
108b2c80  0610a0e3  mov r1, #0x6
108b2c84  0b00a0e1  cpy r0, r11
108b2c88  e405ffeb  bl 0x10874420
108b2c8c  0a00c4e5  strb r0, [r4,#0xa]
108b2c90  0710a0e3  mov r1, #0x7
108b2c94  0b00a0e1  cpy r0, r11
108b2c98  e005ffeb  bl 0x10874420
108b2c9c  0b00c4e5  strb r0, [r4,#0xb]
108b2ca0  0b00a0e1  cpy r0, r11
108b2ca4  5501d0e5  ldrb r0, [r0,#0x155]
108b2ca8  0c00c4e5  strb r0, [r4,#0xc]
108b2cac  0410a0e1  cpy r1, r4
108b2cb0  0900a0e1  cpy r0, r9
108b2cb4  a3d3feeb  bl 0x10867b48
108b2cb8  0520a0e1  cpy r2, r5
108b2cbc  0310a0e3  mov r1, #0x3
108b2cc0  0900a0e1  cpy r0, r9
108b2cc4  acdcfeeb  bl 0x10869f7c
108b2cc8  0040a0e1  cpy r4, r0
108b2ccc  a92fa0e3  mov r2, #0x2a4
108b2cd0  0210a0e3  mov r1, #0x2
108b2cd4  040080e2  add r0, r0, #0x4
108b2cd8  7d69feeb  bl 0x1084d2d4
108b2cdc  0510a0e1  cpy r1, r5
108b2ce0  040084e2  add r0, r4, #0x4
108b2ce4  6c69feeb  bl 0x1084d29c
108b2ce8  0410a0e1  cpy r1, r4
108b2cec  0900a0e1  cpy r0, r9
108b2cf0  f05fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108b2cf4  93d3feea  b 0x10867b48
108b2cf8  f09fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,pc}
