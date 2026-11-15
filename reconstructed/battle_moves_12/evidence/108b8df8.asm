108b8df8  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b8dfc  0060a0e1  cpy r6, r0
108b8e00  0150a0e1  cpy r5, r1
108b8e04  0240a0e1  cpy r4, r2
108b8e08  0370a0e1  cpy r7, r3
108b8e0c  0200a0e3  mov r0, #0x2
108b8e10  c1cafeeb  bl 0x1086b91c
108b8e14  040050e1  cmp r0, r4
108b8e18  2700001a  bne 0x108b8ebc
108b8e1c  0680a0e1  cpy r8, r6
108b8e20  0420a0e1  cpy r2, r4
108b8e24  0a10a0e3  mov r1, #0xa
108b8e28  0500a0e1  cpy r0, r5
108b8e2c  52c4feeb  bl 0x10869f7c
108b8e30  0010a0e1  cpy r1, r0
108b8e34  0120a0e3  mov r2, #0x1
108b8e38  0540c1e5  strb r4, [r1,#0x5]
108b8e3c  1900a0e3  mov r0, #0x19
108b8e40  1120c1e5  strb r2, [r1,#0x11]
108b8e44  0400c1e5  strb r0, [r1,#0x4]
108b8e48  0500a0e1  cpy r0, r5
108b8e4c  3dbbfeeb  bl 0x10867b48
108b8e50  0000a0e3  mov r0, #0x0
108b8e54  180087e5  str r0, [r7,#0x18]
108b8e58  0460a0e1  cpy r6, r4
108b8e5c  0800a0e1  cpy r0, r8
108b8e60  b803d0e1  ldrh r0, [r0,#0x38]
108b8e64  0050a0e1  cpy r5, r0
108b8e68  0610a0e1  cpy r1, r6
108b8e6c  0000a0e3  mov r0, #0x0
108b8e70  5251feeb  bl 0x1084d3c0
108b8e74  0040b0e1  movs r4, r0
108b8e78  0f00000a  beq 0x108b8ebc
108b8e7c  00f020e3  nop
108b8e80  b803d0e1  ldrh r0, [r0,#0x38]
108b8e84  050050e1  cmp r0, r5
108b8e88  0400a0e1  cpy r0, r4
108b8e8c  0600001a  bne 0x108b8eac
108b8e90  cd50feeb  bl 0x1084d1cc
108b8e94  0610a0e1  cpy r1, r6
108b8e98  0000a0e3  mov r0, #0x0
108b8e9c  4751feeb  bl 0x1084d3c0
108b8ea0  00f020e3  nop
108b8ea4  00f020e3  nop
108b8ea8  010000ea  b 0x108b8eb4
108b8eac  00f020e3  nop
108b8eb0  2e51feeb  bl 0x1084d370
108b8eb4  0040b0e1  movs r4, r0
108b8eb8  efffff1a  bne 0x108b8e7c
108b8ebc  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
