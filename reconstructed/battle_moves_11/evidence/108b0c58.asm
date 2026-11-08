108b0c58  ff4f2de9  stmdb sp!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,lr}
108b0c5c  0cd04de2  sub sp, sp, #0xc
108b0c60  0200a0e3  mov r0, #0x2
108b0c64  02a0a0e1  cpy r10, r2
108b0c68  2bebfeeb  bl 0x1086b91c
108b0c6c  0a0050e1  cmp r0, r10
108b0c70  4d00001a  bne 0x108b0dac
108b0c74  10009de5  ldr r0, [sp,#0x10]
108b0c78  0a10a0e1  cpy r1, r10
108b0c7c  336bfeeb  bl 0x1084b950
108b0c80  04008de5  str r0, [sp,#0x4]
108b0c84  10009de5  ldr r0, [sp,#0x10]
108b0c88  3010a0e3  mov r1, #0x30
108b0c8c  cf6efeeb  bl 0x1084c7d0
108b0c90  0070a0e1  cpy r7, r0
108b0c94  04009de5  ldr r0, [sp,#0x4]
108b0c98  0050a0e3  mov r5, #0x0
108b0c9c  1800d0e5  ldrb r0, [r0,#0x18]
108b0ca0  00b050e2  subs r11, r0, #0x0
108b0ca4  0090a0e3  mov r9, #0x0
108b0ca8  3f00009a  bls 0x108b0dac
108b0cac  04009de5  ldr r0, [sp,#0x4]
108b0cb0  0910a0e1  cpy r1, r9
108b0cb4  8fb3feeb  bl 0x1085daf8
108b0cb8  00008de5  str r0, [sp,#0x0]
108b0cbc  00f020e3  nop
108b0cc0  1900d0e5  ldrb r0, [r0,#0x19]
108b0cc4  0a0050e1  cmp r0, r10
108b0cc8  00f020e3  nop
108b0ccc  1f00000a  beq 0x108b0d50
108b0cd0  00009de5  ldr r0, [sp,#0x0]
108b0cd4  dabafeeb  bl 0x1085f844
108b0cd8  000050e3  cmp r0, #0x0
108b0cdc  00f020e3  nop
108b0ce0  0400001a  bne 0x108b0cf8
108b0ce4  00009de5  ldr r0, [sp,#0x0]
108b0ce8  3991feeb  bl 0x108551d4
108b0cec  000050e3  cmp r0, #0x0
108b0cf0  00f020e3  nop
108b0cf4  1500000a  beq 0x108b0d50
108b0cf8  00009de5  ldr r0, [sp,#0x0]
108b0cfc  5201d0e5  ldrb r0, [r0,#0x152]
108b0d00  006050e2  subs r6, r0, #0x0
108b0d04  0040a0e3  mov r4, #0x0
108b0d08  1000009a  bls 0x108b0d50
108b0d0c  00009de5  ldr r0, [sp,#0x0]
108b0d10  0410a0e1  cpy r1, r4
108b0d14  d393feeb  bl 0x10855c68
108b0d18  0080a0e1  cpy r8, r0
108b0d1c  00f020e3  nop
108b0d20  a3ceffeb  bl 0x108a47b4
108b0d24  000050e3  cmp r0, #0x0
108b0d28  00f020e3  nop
108b0d2c  0300001a  bne 0x108b0d40
108b0d30  851087e0  add r1, r7, r5, lsl #0x1
108b0d34  010085e2  add r0, r5, #0x1
108b0d38  ff5000e2  and r5, r0, #0xff
108b0d3c  b080c1e1  strh r8, [r1,#0x0]
108b0d40  010084e2  add r0, r4, #0x1
108b0d44  ff4000e2  and r4, r0, #0xff
108b0d48  060054e1  cmp r4, r6
108b0d4c  eeffff3a  bcc 0x108b0d0c
108b0d50  010089e2  add r0, r9, #0x1
108b0d54  ff9000e2  and r9, r0, #0xff
108b0d58  09005be1  cmp r11, r9
108b0d5c  d2ffff8a  bhi 0x108b0cac
108b0d60  000055e3  cmp r5, #0x0
108b0d64  1000000a  beq 0x108b0dac
108b0d68  0500a0e1  cpy r0, r5
108b0d6c  926efeeb  bl 0x1084c7bc
108b0d70  ff0000e2  and r0, r0, #0xff
108b0d74  0a10a0e1  cpy r1, r10
108b0d78  804087e0  add r4, r7, r0, lsl #0x1
108b0d7c  10009de5  ldr r0, [sp,#0x10]
108b0d80  b020d4e1  ldrh r2, [r4,#0x0]
108b0d84  8803ffeb  bl 0x10871bac
108b0d88  0050a0e1  cpy r5, r0
108b0d8c  b010d4e1  ldrh r1, [r4,#0x0]
108b0d90  1200a0e3  mov r0, #0x12
108b0d94  ced2feeb  bl 0x108658d4
108b0d98  1cd08de2  add sp, sp, #0x1c
108b0d9c  0510a0e1  cpy r1, r5
108b0da0  f04fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,lr}
108b0da4  0d00a0e3  mov r0, #0xd
108b0da8  c9d2feea  b 0x108658d4
108b0dac  1cd08de2  add sp, sp, #0x1c
108b0db0  f08fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,pc}
