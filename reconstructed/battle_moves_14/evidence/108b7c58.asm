108b7c58  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b7c5c  0060a0e1  cpy r6, r0
108b7c60  0150a0e1  cpy r5, r1
108b7c64  0240a0e1  cpy r4, r2
108b7c68  0370a0e1  cpy r7, r3
108b7c6c  0200a0e3  mov r0, #0x2
108b7c70  29cffeeb  bl 0x1086b91c
108b7c74  040050e1  cmp r0, r4
108b7c78  2b00001a  bne 0x108b7d2c
108b7c7c  5300a0e3  mov r0, #0x53
108b7c80  25cffeeb  bl 0x1086b91c
108b7c84  000050e3  cmp r0, #0x0
108b7c88  2700001a  bne 0x108b7d2c
108b7c8c  0680a0e1  cpy r8, r6
108b7c90  0420a0e1  cpy r2, r4
108b7c94  0a10a0e3  mov r1, #0xa
108b7c98  0500a0e1  cpy r0, r5
108b7c9c  b6c8feeb  bl 0x10869f7c
108b7ca0  0010a0e1  cpy r1, r0
108b7ca4  0120a0e3  mov r2, #0x1
108b7ca8  0540c1e5  strb r4, [r1,#0x5]
108b7cac  1900a0e3  mov r0, #0x19
108b7cb0  1120c1e5  strb r2, [r1,#0x11]
108b7cb4  0400c1e5  strb r0, [r1,#0x4]
108b7cb8  0500a0e1  cpy r0, r5
108b7cbc  a1bffeeb  bl 0x10867b48
108b7cc0  0000a0e3  mov r0, #0x0
108b7cc4  180087e5  str r0, [r7,#0x18]
108b7cc8  0460a0e1  cpy r6, r4
108b7ccc  0800a0e1  cpy r0, r8
108b7cd0  b803d0e1  ldrh r0, [r0,#0x38]
108b7cd4  0050a0e1  cpy r5, r0
108b7cd8  0610a0e1  cpy r1, r6
108b7cdc  0000a0e3  mov r0, #0x0
108b7ce0  b655feeb  bl 0x1084d3c0
108b7ce4  0040b0e1  movs r4, r0
108b7ce8  0f00000a  beq 0x108b7d2c
108b7cec  00f020e3  nop
108b7cf0  b803d0e1  ldrh r0, [r0,#0x38]
108b7cf4  050050e1  cmp r0, r5
108b7cf8  0400a0e1  cpy r0, r4
108b7cfc  0600001a  bne 0x108b7d1c
108b7d00  3155feeb  bl 0x1084d1cc
108b7d04  0610a0e1  cpy r1, r6
108b7d08  0000a0e3  mov r0, #0x0
108b7d0c  ab55feeb  bl 0x1084d3c0
108b7d10  00f020e3  nop
108b7d14  00f020e3  nop
108b7d18  010000ea  b 0x108b7d24
108b7d1c  00f020e3  nop
108b7d20  9255feeb  bl 0x1084d370
108b7d24  0040b0e1  movs r4, r0
108b7d28  efffff1a  bne 0x108b7cec
108b7d2c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
