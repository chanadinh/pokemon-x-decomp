108b4db8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b4dbc  0160a0e1  cpy r6, r1
108b4dc0  0250a0e1  cpy r5, r2
108b4dc4  0400a0e3  mov r0, #0x4
108b4dc8  d3dafeeb  bl 0x1086b91c
108b4dcc  050050e1  cmp r0, r5
108b4dd0  2200001a  bne 0x108b4e60
108b4dd4  4700a0e3  mov r0, #0x47
108b4dd8  cfdafeeb  bl 0x1086b91c
108b4ddc  000050e3  cmp r0, #0x0
108b4de0  1e00001a  bne 0x108b4e60
108b4de4  0510a0e1  cpy r1, r5
108b4de8  0600a0e1  cpy r0, r6
108b4dec  ab7dfeeb  bl 0x108544a0
108b4df0  93aafeeb  bl 0x1085f844
108b4df4  000050e3  cmp r0, #0x0
108b4df8  1800001a  bne 0x108b4e60
108b4dfc  0520a0e1  cpy r2, r5
108b4e00  0d10a0e3  mov r1, #0xd
108b4e04  0600a0e1  cpy r0, r6
108b4e08  5bd4feeb  bl 0x10869f7c
108b4e0c  0040a0e1  cpy r4, r0
108b4e10  1800d0e5  ldrb r0, [r0,#0x18]
108b4e14  0110a0e3  mov r1, #0x1
108b4e18  0c50c4e5  strb r5, [r4,#0xc]
108b4e1c  0410c4e5  strb r1, [r4,#0x4]
108b4e20  1710c4e5  strb r1, [r4,#0x17]
108b4e24  0200c0e3  bic r0, r0, #0x2
108b4e28  1610c4e5  strb r1, [r4,#0x16]
108b4e2c  050080e3  orr r0, r0, #0x5
108b4e30  1800c4e5  strb r0, [r4,#0x18]
108b4e34  852fa0e3  mov r2, #0x214
108b4e38  0210a0e3  mov r1, #0x2
108b4e3c  1c0084e2  add r0, r4, #0x1c
108b4e40  2361feeb  bl 0x1084d2d4
108b4e44  0510a0e1  cpy r1, r5
108b4e48  1c0084e2  add r0, r4, #0x1c
108b4e4c  1261feeb  bl 0x1084d29c
108b4e50  0410a0e1  cpy r1, r4
108b4e54  0600a0e1  cpy r0, r6
108b4e58  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b4e5c  39cbfeea  b 0x10867b48
108b4e60  7080bde8  ldmia sp!,{r4,r5,r6,pc}
