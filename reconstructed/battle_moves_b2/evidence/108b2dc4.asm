108b2dc4  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b2dc8  0150a0e1  cpy r5, r1
108b2dcc  0240a0e1  cpy r4, r2
108b2dd0  0300a0e3  mov r0, #0x3
108b2dd4  d0e2feeb  bl 0x1086b91c
108b2dd8  040050e1  cmp r0, r4
108b2ddc  1200001a  bne 0x108b2e2c
108b2de0  0420a0e1  cpy r2, r4
108b2de4  3210a0e3  mov r1, #0x32
108b2de8  0500a0e1  cpy r0, r5
108b2dec  62dcfeeb  bl 0x10869f7c
108b2df0  0040a0e1  cpy r4, r0
108b2df4  0600a0e3  mov r0, #0x6
108b2df8  c7e2feeb  bl 0x1086b91c
108b2dfc  0400c4e5  strb r0, [r4,#0x4]
108b2e00  28209fe5  ldr r2, [0x108b2e30]
108b2e04  0210a0e3  mov r1, #0x2
108b2e08  080084e2  add r0, r4, #0x8
108b2e0c  3069feeb  bl 0x1084d2d4
108b2e10  0410d4e5  ldrb r1, [r4,#0x4]
108b2e14  080084e2  add r0, r4, #0x8
108b2e18  1f69feeb  bl 0x1084d29c
108b2e1c  0410a0e1  cpy r1, r4
108b2e20  0500a0e1  cpy r0, r5
108b2e24  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b2e28  46d3feea  b 0x10867b48
108b2e2c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
