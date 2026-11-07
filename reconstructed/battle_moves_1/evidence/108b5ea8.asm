108b5ea8  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b5eac  0140a0e1  cpy r4, r1
108b5eb0  0070a0e1  cpy r7, r0
108b5eb4  0260a0e1  cpy r6, r2
108b5eb8  0350a0e1  cpy r5, r3
108b5ebc  0300a0e3  mov r0, #0x3
108b5ec0  95d6feeb  bl 0x1086b91c
108b5ec4  060050e1  cmp r0, r6
108b5ec8  3600001a  bne 0x108b5fa8
108b5ecc  0610a0e1  cpy r1, r6
108b5ed0  0400a0e1  cpy r0, r4
108b5ed4  7179feeb  bl 0x108544a0
108b5ed8  0020a0e1  cpy r2, r0
108b5edc  000095e5  ldr r0, [r5,#0x0]
108b5ee0  0140a0e3  mov r4, #0x1
108b5ee4  000050e3  cmp r0, #0x0
108b5ee8  0600000a  beq 0x108b5f08
108b5eec  010010e3  tst r0, #0x1
108b5ef0  0240a013  movne r4, #0x2
108b5ef4  a000b0e1  movs r0, r0, lsr #0x1
108b5ef8  0200000a  beq 0x108b5f08
108b5efc  010050e2  subs r0, r0, #0x1
108b5f00  0441a0e1  mov r4, r4, lsl #0x2
108b5f04  fcffff1a  bne 0x108b5efc
108b5f08  0710a0e3  mov r1, #0x7
108b5f0c  0200a0e1  cpy r0, r2
108b5f10  cd5afeeb  bl 0x1084ca4c
108b5f14  000050e3  cmp r0, #0x0
108b5f18  8440a011  movne r4, r4, lsl #0x1
108b5f1c  3100a0e3  mov r0, #0x31
108b5f20  7dd6feeb  bl 0x1086b91c
108b5f24  900401e0  mul r1, r0, r4
108b5f28  3100a0e3  mov r0, #0x31
108b5f2c  68befeeb  bl 0x108658d4
108b5f30  d000c5e1  ldrd r0, r1, [r5,#0x0]
108b5f34  010080e2  add r0, r0, #0x1
108b5f38  000051e1  cmp r1, r0
108b5f3c  000085e5  str r0, [r5,#0x0]
108b5f40  180000ca  bgt 0x108b5fa8
108b5f44  0700a0e1  cpy r0, r7
108b5f48  b803d0e1  ldrh r0, [r0,#0x38]
108b5f4c  0050a0e1  cpy r5, r0
108b5f50  0610a0e1  cpy r1, r6
108b5f54  0000a0e3  mov r0, #0x0
108b5f58  185dfeeb  bl 0x1084d3c0
108b5f5c  0040b0e1  movs r4, r0
108b5f60  00f020e3  nop
108b5f64  0f00000a  beq 0x108b5fa8
108b5f68  00f020e3  nop
108b5f6c  b803d0e1  ldrh r0, [r0,#0x38]
108b5f70  050050e1  cmp r0, r5
108b5f74  0400a0e1  cpy r0, r4
108b5f78  0600001a  bne 0x108b5f98
108b5f7c  925cfeeb  bl 0x1084d1cc
108b5f80  0610a0e1  cpy r1, r6
108b5f84  0000a0e3  mov r0, #0x0
108b5f88  0c5dfeeb  bl 0x1084d3c0
108b5f8c  00f020e3  nop
108b5f90  00f020e3  nop
108b5f94  010000ea  b 0x108b5fa0
108b5f98  00f020e3  nop
108b5f9c  f35cfeeb  bl 0x1084d370
108b5fa0  0040b0e1  movs r4, r0
108b5fa4  efffff1a  bne 0x108b5f68
108b5fa8  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
