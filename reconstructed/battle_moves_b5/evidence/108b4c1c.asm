108b4c1c  f05f2de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108b4c20  0150a0e1  cpy r5, r1
108b4c24  0240a0e1  cpy r4, r2
108b4c28  0300a0e3  mov r0, #0x3
108b4c2c  3adbfeeb  bl 0x1086b91c
108b4c30  040050e1  cmp r0, r4
108b4c34  2700001a  bne 0x108b4cd8
108b4c38  0580a0e1  cpy r8, r5
108b4c3c  0470a0e1  cpy r7, r4
108b4c40  0b90a0e3  mov r9, #0xb
108b4c44  0500a0e3  mov r0, #0x5
108b4c48  33dbfeeb  bl 0x1086b91c
108b4c4c  006050e2  subs r6, r0, #0x0
108b4c50  0040a0e3  mov r4, #0x0
108b4c54  01a0a083  movhi r10, #0x1
108b4c58  02b0a083  movhi r11, #0x2
108b4c5c  1d00009a  bls 0x108b4cd8
108b4c60  060084e2  add r0, r4, #0x6
108b4c64  ff0000e2  and r0, r0, #0xff
108b4c68  2bdbfeeb  bl 0x1086b91c
108b4c6c  ff5000e2  and r5, r0, #0xff
108b4c70  0510a0e1  cpy r1, r5
108b4c74  0800a0e1  cpy r0, r8
108b4c78  087efeeb  bl 0x108544a0
108b4c7c  0910a0e1  cpy r1, r9
108b4c80  00f020e3  nop
108b4c84  35ebfeeb  bl 0x1086f960
108b4c88  000050e3  cmp r0, #0x0
108b4c8c  00f020e3  nop
108b4c90  0d00000a  beq 0x108b4ccc
108b4c94  0720a0e1  cpy r2, r7
108b4c98  0d10a0e3  mov r1, #0xd
108b4c9c  0800a0e1  cpy r0, r8
108b4ca0  b5d4feeb  bl 0x10869f7c
108b4ca4  1820d0e5  ldrb r2, [r0,#0x18]
108b4ca8  0c50c0e5  strb r5, [r0,#0xc]
108b4cac  16a0c0e5  strb r10, [r0,#0x16]
108b4cb0  0010a0e1  cpy r1, r0
108b4cb4  04b0c0e5  strb r11, [r0,#0x4]
108b4cb8  260082e3  orr r0, r2, #0x26
108b4cbc  17a0c1e5  strb r10, [r1,#0x17]
108b4cc0  1800c1e5  strb r0, [r1,#0x18]
108b4cc4  0800a0e1  cpy r0, r8
108b4cc8  9ecbfeeb  bl 0x10867b48
108b4ccc  014084e2  add r4, r4, #0x1
108b4cd0  060054e1  cmp r4, r6
108b4cd4  e1ffff3a  bcc 0x108b4c60
108b4cd8  f09fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,pc}
