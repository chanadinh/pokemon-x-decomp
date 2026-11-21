108b1c7c  f05f2de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108b1c80  0260a0e1  cpy r6, r2
108b1c84  0300a0e3  mov r0, #0x3
108b1c88  0190a0e1  cpy r9, r1
108b1c8c  22e7feeb  bl 0x1086b91c
108b1c90  060050e1  cmp r0, r6
108b1c94  6a00001a  bne 0x108b1e44
108b1c98  0600a0e3  mov r0, #0x6
108b1c9c  1ee7feeb  bl 0x1086b91c
108b1ca0  ff7000e2  and r7, r0, #0xff
108b1ca4  0710a0e1  cpy r1, r7
108b1ca8  0900a0e1  cpy r0, r9
108b1cac  fb89feeb  bl 0x108544a0
108b1cb0  00b0a0e1  cpy r11, r0
108b1cb4  0610a0e1  cpy r1, r6
108b1cb8  0900a0e1  cpy r0, r9
108b1cbc  f789feeb  bl 0x108544a0
108b1cc0  00a0a0e1  cpy r10, r0
108b1cc4  1c10a0e3  mov r1, #0x1c
108b1cc8  0900a0e1  cpy r0, r9
108b1ccc  bf6afeeb  bl 0x1084c7d0
108b1cd0  0050a0e1  cpy r5, r0
108b1cd4  0a00a0e1  cpy r0, r10
108b1cd8  5501d0e5  ldrb r0, [r0,#0x155]
108b1cdc  0080a0e1  cpy r8, r0
108b1ce0  0140a0e3  mov r4, #0x1
108b1ce4  ff1004e2  and r1, r4, #0xff
108b1ce8  0a00a0e1  cpy r0, r10
108b1cec  cb09ffeb  bl 0x10874420
108b1cf0  041185e0  add r1, r5, r4, lsl #0x2
108b1cf4  014084e2  add r4, r4, #0x1
108b1cf8  070054e3  cmp r4, #0x7
108b1cfc  040001e5  str r0, [r1,#-0x4]
108b1d00  f7ffffda  ble 0x108b1ce4
108b1d04  0620a0e1  cpy r2, r6
108b1d08  0e10a0e3  mov r1, #0xe
108b1d0c  0900a0e1  cpy r0, r9
108b1d10  99e0feeb  bl 0x10869f7c
108b1d14  0040a0e1  cpy r4, r0
108b1d18  0460c0e5  strb r6, [r0,#0x4]
108b1d1c  0110a0e3  mov r1, #0x1
108b1d20  0b00a0e1  cpy r0, r11
108b1d24  bd09ffeb  bl 0x10874420
108b1d28  0500c4e5  strb r0, [r4,#0x5]
108b1d2c  0210a0e3  mov r1, #0x2
108b1d30  0b00a0e1  cpy r0, r11
108b1d34  b909ffeb  bl 0x10874420
108b1d38  0600c4e5  strb r0, [r4,#0x6]
108b1d3c  0310a0e3  mov r1, #0x3
108b1d40  0b00a0e1  cpy r0, r11
108b1d44  b509ffeb  bl 0x10874420
108b1d48  0700c4e5  strb r0, [r4,#0x7]
108b1d4c  0410a0e3  mov r1, #0x4
108b1d50  0b00a0e1  cpy r0, r11
108b1d54  b109ffeb  bl 0x10874420
108b1d58  0800c4e5  strb r0, [r4,#0x8]
108b1d5c  0510a0e3  mov r1, #0x5
108b1d60  0b00a0e1  cpy r0, r11
108b1d64  ad09ffeb  bl 0x10874420
108b1d68  0900c4e5  strb r0, [r4,#0x9]
108b1d6c  0610a0e3  mov r1, #0x6
108b1d70  0b00a0e1  cpy r0, r11
108b1d74  a909ffeb  bl 0x10874420
108b1d78  0a00c4e5  strb r0, [r4,#0xa]
108b1d7c  0710a0e3  mov r1, #0x7
108b1d80  0b00a0e1  cpy r0, r11
108b1d84  a509ffeb  bl 0x10874420
108b1d88  0b00c4e5  strb r0, [r4,#0xb]
108b1d8c  0410a0e1  cpy r1, r4
108b1d90  0900a0e1  cpy r0, r9
108b1d94  0c80c4e5  strb r8, [r4,#0xc]
108b1d98  6ad7feeb  bl 0x10867b48
108b1d9c  0620a0e1  cpy r2, r6
108b1da0  0e10a0e3  mov r1, #0xe
108b1da4  0900a0e1  cpy r0, r9
108b1da8  73e0feeb  bl 0x10869f7c
108b1dac  0040a0e1  cpy r4, r0
108b1db0  0470c0e5  strb r7, [r0,#0x4]
108b1db4  000095e5  ldr r0, [r5,#0x0]
108b1db8  0500c4e5  strb r0, [r4,#0x5]
108b1dbc  040095e5  ldr r0, [r5,#0x4]
108b1dc0  0600c4e5  strb r0, [r4,#0x6]
108b1dc4  080095e5  ldr r0, [r5,#0x8]
108b1dc8  0700c4e5  strb r0, [r4,#0x7]
108b1dcc  0c0095e5  ldr r0, [r5,#0xc]
108b1dd0  0800c4e5  strb r0, [r4,#0x8]
108b1dd4  100095e5  ldr r0, [r5,#0x10]
108b1dd8  0900c4e5  strb r0, [r4,#0x9]
108b1ddc  140095e5  ldr r0, [r5,#0x14]
108b1de0  0a00c4e5  strb r0, [r4,#0xa]
108b1de4  180095e5  ldr r0, [r5,#0x18]
108b1de8  0b00c4e5  strb r0, [r4,#0xb]
108b1dec  0b00a0e1  cpy r0, r11
108b1df0  5501d0e5  ldrb r0, [r0,#0x155]
108b1df4  0c00c4e5  strb r0, [r4,#0xc]
108b1df8  0410a0e1  cpy r1, r4
108b1dfc  0900a0e1  cpy r0, r9
108b1e00  50d7feeb  bl 0x10867b48
108b1e04  0620a0e1  cpy r2, r6
108b1e08  0310a0e3  mov r1, #0x3
108b1e0c  0900a0e1  cpy r0, r9
108b1e10  59e0feeb  bl 0x10869f7c
108b1e14  0040a0e1  cpy r4, r0
108b1e18  28209fe5  ldr r2, [0x108b1e48]
108b1e1c  0210a0e3  mov r1, #0x2
108b1e20  040080e2  add r0, r0, #0x4
108b1e24  2a6dfeeb  bl 0x1084d2d4
108b1e28  0610a0e1  cpy r1, r6
108b1e2c  040084e2  add r0, r4, #0x4
108b1e30  196dfeeb  bl 0x1084d29c
108b1e34  0410a0e1  cpy r1, r4
108b1e38  0900a0e1  cpy r0, r9
108b1e3c  f05fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108b1e40  40d7feea  b 0x10867b48
108b1e44  f09fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,pc}
