108b7db8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b7dbc  0150a0e1  cpy r5, r1
108b7dc0  0240a0e1  cpy r4, r2
108b7dc4  0300a0e3  mov r0, #0x3
108b7dc8  d3cefeeb  bl 0x1086b91c
108b7dcc  040050e1  cmp r0, r4
108b7dd0  1c00001a  bne 0x108b7e48
108b7dd4  0410a0e1  cpy r1, r4
108b7dd8  0500a0e1  cpy r0, r5
108b7ddc  af71feeb  bl 0x108544a0
108b7de0  8552feeb  bl 0x1084c7fc
108b7de4  60309fe5  ldr r3, [0x108b7e4c]
108b7de8  0010a0e1  cpy r1, r0
108b7dec  0000a0e3  mov r0, #0x0
108b7df0  802083e0  add r2, r3, r0, lsl #0x1
108b7df4  b0c0d2e1  ldrh r12, [r2,#0x0]
108b7df8  01005ce1  cmp r12, r1
108b7dfc  b220d211  ldrhne r2, [r2,#0x2]
108b7e00  01005211  cmpne r2, r1
108b7e04  0f00000a  beq 0x108b7e48
108b7e08  020080e2  add r0, r0, #0x2
108b7e0c  080050e3  cmp r0, #0x8
108b7e10  f6ffff3a  bcc 0x108b7df0
108b7e14  0420a0e1  cpy r2, r4
108b7e18  2710a0e3  mov r1, #0x27
108b7e1c  0500a0e1  cpy r0, r5
108b7e20  55c8feeb  bl 0x10869f7c
108b7e24  0010a0e1  cpy r1, r0
108b7e28  0320a0e3  mov r2, #0x3
108b7e2c  0520c1e5  strb r2, [r1,#0x5]
108b7e30  0000a0e3  mov r0, #0x0
108b7e34  0440c1e5  strb r4, [r1,#0x4]
108b7e38  0600c1e5  strb r0, [r1,#0x6]
108b7e3c  0500a0e1  cpy r0, r5
108b7e40  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b7e44  3fbffeea  b 0x10867b48
108b7e48  7080bde8  ldmia sp!,{r4,r5,r6,pc}
