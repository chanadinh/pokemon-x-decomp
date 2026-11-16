108b6d8c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b6d90  0060a0e1  cpy r6, r0
108b6d94  180093e5  ldr r0, [r3,#0x18]
108b6d98  0150a0e1  cpy r5, r1
108b6d9c  0240a0e1  cpy r4, r2
108b6da0  000050e3  cmp r0, #0x0
108b6da4  2600000a  beq 0x108b6e44
108b6da8  0200a0e3  mov r0, #0x2
108b6dac  dad2feeb  bl 0x1086b91c
108b6db0  040050e1  cmp r0, r4
108b6db4  2200001a  bne 0x108b6e44
108b6db8  0c00a0e3  mov r0, #0xc
108b6dbc  d6d2feeb  bl 0x1086b91c
108b6dc0  080050e3  cmp r0, #0x8
108b6dc4  1e00000a  beq 0x108b6e44
108b6dc8  0410a0e1  cpy r1, r4
108b6dcc  0500a0e1  cpy r0, r5
108b6dd0  b275feeb  bl 0x108544a0
108b6dd4  0040a0e1  cpy r4, r0
108b6dd8  0600a0e1  cpy r0, r6
108b6ddc  b803d0e1  ldrh r0, [r0,#0x38]
108b6de0  0050a0e1  cpy r5, r0
108b6de4  0400a0e1  cpy r0, r4
108b6de8  1900d0e5  ldrb r0, [r0,#0x19]
108b6dec  0060a0e1  cpy r6, r0
108b6df0  0010a0e1  cpy r1, r0
108b6df4  0000a0e3  mov r0, #0x0
108b6df8  7059feeb  bl 0x1084d3c0
108b6dfc  0040b0e1  movs r4, r0
108b6e00  0f00000a  beq 0x108b6e44
108b6e04  00f020e3  nop
108b6e08  b803d0e1  ldrh r0, [r0,#0x38]
108b6e0c  050050e1  cmp r0, r5
108b6e10  0400a0e1  cpy r0, r4
108b6e14  0600001a  bne 0x108b6e34
108b6e18  eb58feeb  bl 0x1084d1cc
108b6e1c  0610a0e1  cpy r1, r6
108b6e20  0000a0e3  mov r0, #0x0
108b6e24  6559feeb  bl 0x1084d3c0
108b6e28  00f020e3  nop
108b6e2c  00f020e3  nop
108b6e30  010000ea  b 0x108b6e3c
108b6e34  00f020e3  nop
108b6e38  4c59feeb  bl 0x1084d370
108b6e3c  0040b0e1  movs r4, r0
108b6e40  efffff1a  bne 0x108b6e04
108b6e44  7080bde8  ldmia sp!,{r4,r5,r6,pc}
