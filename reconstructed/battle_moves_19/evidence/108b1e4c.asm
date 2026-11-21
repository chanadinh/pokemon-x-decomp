108b1e4c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b1e50  0160a0e1  cpy r6, r1
108b1e54  0250a0e1  cpy r5, r2
108b1e58  0300a0e3  mov r0, #0x3
108b1e5c  aee6feeb  bl 0x1086b91c
108b1e60  050050e1  cmp r0, r5
108b1e64  4000001a  bne 0x108b1f6c
108b1e68  0600a0e1  cpy r0, r6
108b1e6c  6a66feeb  bl 0x1084b81c
108b1e70  080050e3  cmp r0, #0x8
108b1e74  0040a0e3  mov r4, #0x0
108b1e78  00f19f37  ldrcc pc, [pc,r0,lsl #0x2]
108b1e7c  100000ea  b 0x108b1ec4
108b1ea0  0440a0e3  mov r4, #0x4
108b1ea4  060000ea  b 0x108b1ec4
108b1ea8  0540a0e3  mov r4, #0x5
108b1eac  040000ea  b 0x108b1ec4
108b1eb0  0b40a0e3  mov r4, #0xb
108b1eb4  020000ea  b 0x108b1ec4
108b1eb8  0a40a0e3  mov r4, #0xa
108b1ebc  000000ea  b 0x108b1ec4
108b1ec0  0e40a0e3  mov r4, #0xe
108b1ec4  0600a0e1  cpy r0, r6
108b1ec8  8166feeb  bl 0x1084b8d4
108b1ecc  060050e3  cmp r0, #0x6
108b1ed0  00f19f37  ldrcc pc, [pc,r0,lsl #0x2]
108b1ed4  0e0000ea  b 0x108b1f14
108b1ef0  0b40a0e3  mov r4, #0xb
108b1ef4  060000ea  b 0x108b1f14
108b1ef8  0c40a0e3  mov r4, #0xc
108b1efc  040000ea  b 0x108b1f14
108b1f00  1140a0e3  mov r4, #0x11
108b1f04  020000ea  b 0x108b1f14
108b1f08  0a40a0e3  mov r4, #0xa
108b1f0c  000000ea  b 0x108b1f14
108b1f10  0e40a0e3  mov r4, #0xe
108b1f14  1f0004e2  and r0, r4, #0x1f
108b1f18  0510a0e1  cpy r1, r5
108b1f1c  800280e1  orr r0, r0, r0, lsl #0x5
108b1f20  124b80e3  orr r4, r0, #0x4800
108b1f24  0600a0e1  cpy r0, r6
108b1f28  5c89feeb  bl 0x108544a0
108b1f2c  00f020e3  nop
108b1f30  00f020e3  nop
108b1f34  96fafeeb  bl 0x10870994
108b1f38  040050e1  cmp r0, r4
108b1f3c  00f020e3  nop
108b1f40  0900000a  beq 0x108b1f6c
108b1f44  0520a0e1  cpy r2, r5
108b1f48  1310a0e3  mov r1, #0x13
108b1f4c  0600a0e1  cpy r0, r6
108b1f50  09e0feeb  bl 0x10869f7c
108b1f54  b440c0e1  strh r4, [r0,#0x4]
108b1f58  0010a0e1  cpy r1, r0
108b1f5c  0650c0e5  strb r5, [r0,#0x6]
108b1f60  0600a0e1  cpy r0, r6
108b1f64  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b1f68  f6d6feea  b 0x10867b48
108b1f6c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
