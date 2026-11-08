108c2c58  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c2c5c  000093e5  ldr r0, [r3,#0x0]
108c2c60  000050e3  cmp r0, #0x0
108c2c64  1f00000a  beq 0x108c2ce8
108c2c68  0160a0e1  cpy r6, r1
108c2c6c  0250a0e1  cpy r5, r2
108c2c70  0210a0e1  cpy r1, r2
108c2c74  0600a0e1  cpy r0, r6
108c2c78  0846feeb  bl 0x108544a0
108c2c7c  0040a0e1  cpy r4, r0
108c2c80  bc00d0e1  ldrh r0, [r0,#0xc]
108c2c84  011c40e2  sub r1, r0, #0x100
108c2c88  5f1051e2  subs r1, r1, #0x5f
108c2c8c  1500001a  bne 0x108c2ce8
108c2c90  5301d4e5  ldrb r0, [r4,#0x153]
108c2c94  000050e3  cmp r0, #0x0
108c2c98  1200000a  beq 0x108c2ce8
108c2c9c  0520a0e1  cpy r2, r5
108c2ca0  3a10a0e3  mov r1, #0x3a
108c2ca4  0600a0e1  cpy r0, r6
108c2ca8  b39cfeeb  bl 0x10869f7c
108c2cac  0040a0e1  cpy r4, r0
108c2cb0  0000a0e3  mov r0, #0x0
108c2cb4  0450c4e5  strb r5, [r4,#0x4]
108c2cb8  0500c4e5  strb r0, [r4,#0x5]
108c2cbc  de20a0e3  mov r2, #0xde
108c2cc0  0210a0e3  mov r1, #0x2
108c2cc4  080084e2  add r0, r4, #0x8
108c2cc8  8129feeb  bl 0x1084d2d4
108c2ccc  0510a0e1  cpy r1, r5
108c2cd0  080084e2  add r0, r4, #0x8
108c2cd4  7029feeb  bl 0x1084d29c
108c2cd8  0410a0e1  cpy r1, r4
108c2cdc  0600a0e1  cpy r0, r6
108c2ce0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c2ce4  9793feea  b 0x10867b48
108c2ce8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
