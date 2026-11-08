108b6294  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b6298  0150a0e1  cpy r5, r1
108b629c  0240a0e1  cpy r4, r2
108b62a0  0300a0e3  mov r0, #0x3
108b62a4  9cd5feeb  bl 0x1086b91c
108b62a8  040050e1  cmp r0, r4
108b62ac  1100001a  bne 0x108b62f8
108b62b0  0420a0e1  cpy r2, r4
108b62b4  1c10a0e3  mov r1, #0x1c
108b62b8  0500a0e1  cpy r0, r5
108b62bc  2ecffeeb  bl 0x10869f7c
108b62c0  0040a0e1  cpy r4, r0
108b62c4  0800a0e3  mov r0, #0x8
108b62c8  0400c4e5  strb r0, [r4,#0x4]
108b62cc  0100a0e3  mov r0, #0x1
108b62d0  d956feeb  bl 0x1084be3c
108b62d4  080084e5  str r0, [r4,#0x8]
108b62d8  1c209fe5  ldr r2, [0x108b62fc]
108b62dc  0110a0e3  mov r1, #0x1
108b62e0  0c0084e2  add r0, r4, #0xc
108b62e4  fa5bfeeb  bl 0x1084d2d4
108b62e8  0410a0e1  cpy r1, r4
108b62ec  0500a0e1  cpy r0, r5
108b62f0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b62f4  13c6feea  b 0x10867b48
108b62f8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
