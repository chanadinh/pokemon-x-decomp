108c4cac  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c4cb0  0160a0e1  cpy r6, r1
108c4cb4  0250a0e1  cpy r5, r2
108c4cb8  0200a0e3  mov r0, #0x2
108c4cbc  169bfeeb  bl 0x1086b91c
108c4cc0  050050e1  cmp r0, r5
108c4cc4  1900001a  bne 0x108c4d30
108c4cc8  0510a0e1  cpy r1, r5
108c4ccc  0600a0e1  cpy r0, r6
108c4cd0  f23dfeeb  bl 0x108544a0
108c4cd4  bc00d0e1  ldrh r0, [r0,#0xc]
108c4cd8  021c40e2  sub r1, r0, #0x200
108c4cdc  2b1051e2  subs r1, r1, #0x2b
108c4ce0  1200001a  bne 0x108c4d30
108c4ce4  0520a0e1  cpy r2, r5
108c4ce8  3a10a0e3  mov r1, #0x3a
108c4cec  0600a0e1  cpy r0, r6
108c4cf0  a194feeb  bl 0x10869f7c
108c4cf4  0040a0e1  cpy r4, r0
108c4cf8  0000a0e3  mov r0, #0x0
108c4cfc  0450c4e5  strb r5, [r4,#0x4]
108c4d00  0500c4e5  strb r0, [r4,#0x5]
108c4d04  c020a0e3  mov r2, #0xc0
108c4d08  0110a0e3  mov r1, #0x1
108c4d0c  080084e2  add r0, r4, #0x8
108c4d10  6f21feeb  bl 0x1084d2d4
108c4d14  0510a0e1  cpy r1, r5
108c4d18  080084e2  add r0, r4, #0x8
108c4d1c  5e21feeb  bl 0x1084d29c
108c4d20  0410a0e1  cpy r1, r4
108c4d24  0600a0e1  cpy r0, r6
108c4d28  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c4d2c  858bfeea  b 0x10867b48
108c4d30  7080bde8  ldmia sp!,{r4,r5,r6,pc}
