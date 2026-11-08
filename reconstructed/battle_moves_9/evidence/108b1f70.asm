108b1f70  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b1f74  0160a0e1  cpy r6, r1
108b1f78  0070a0e1  cpy r7, r0
108b1f7c  0240a0e1  cpy r4, r2
108b1f80  0350a0e1  cpy r5, r3
108b1f84  0200a0e3  mov r0, #0x2
108b1f88  63e6feeb  bl 0x1086b91c
108b1f8c  040050e1  cmp r0, r4
108b1f90  2700001a  bne 0x108b2034
108b1f94  1200a0e3  mov r0, #0x12
108b1f98  5fe6feeb  bl 0x1086b91c
108b1f9c  0080a0e1  cpy r8, r0
108b1fa0  0700a0e1  cpy r0, r7
108b1fa4  b803d0e1  ldrh r0, [r0,#0x38]
108b1fa8  000058e1  cmp r8, r0
108b1fac  0100a003  moveq r0, #0x1
108b1fb0  18008505  streq r0, [r5,#0x18]
108b1fb4  1e00000a  beq 0x108b2034
108b1fb8  0410a0e1  cpy r1, r4
108b1fbc  0600a0e1  cpy r0, r6
108b1fc0  3689feeb  bl 0x108544a0
108b1fc4  0040a0e1  cpy r4, r0
108b1fc8  0700a0e1  cpy r0, r7
108b1fcc  b803d0e1  ldrh r0, [r0,#0x38]
108b1fd0  0050a0e1  cpy r5, r0
108b1fd4  0400a0e1  cpy r0, r4
108b1fd8  1900d0e5  ldrb r0, [r0,#0x19]
108b1fdc  0060a0e1  cpy r6, r0
108b1fe0  0010a0e1  cpy r1, r0
108b1fe4  0000a0e3  mov r0, #0x0
108b1fe8  f46cfeeb  bl 0x1084d3c0
108b1fec  0040b0e1  movs r4, r0
108b1ff0  0f00000a  beq 0x108b2034
108b1ff4  00f020e3  nop
108b1ff8  b803d0e1  ldrh r0, [r0,#0x38]
108b1ffc  050050e1  cmp r0, r5
108b2000  0400a0e1  cpy r0, r4
108b2004  0600001a  bne 0x108b2024
108b2008  6f6cfeeb  bl 0x1084d1cc
108b200c  0610a0e1  cpy r1, r6
108b2010  0000a0e3  mov r0, #0x0
108b2014  e96cfeeb  bl 0x1084d3c0
108b2018  00f020e3  nop
108b201c  00f020e3  nop
108b2020  010000ea  b 0x108b202c
108b2024  00f020e3  nop
108b2028  d06cfeeb  bl 0x1084d370
108b202c  0040b0e1  movs r4, r0
108b2030  efffff1a  bne 0x108b1ff4
108b2034  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
