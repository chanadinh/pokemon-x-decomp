108b7d30  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b7d34  0150a0e1  cpy r5, r1
108b7d38  0240a0e1  cpy r4, r2
108b7d3c  0200a0e3  mov r0, #0x2
108b7d40  f5cefeeb  bl 0x1086b91c
108b7d44  040050e1  cmp r0, r4
108b7d48  1800001a  bne 0x108b7db0
108b7d4c  0410a0e1  cpy r1, r4
108b7d50  0500a0e1  cpy r0, r5
108b7d54  d171feeb  bl 0x108544a0
108b7d58  0310a0e3  mov r1, #0x3
108b7d5c  724ffeeb  bl 0x1084bb2c
108b7d60  000050e3  cmp r0, #0x0
108b7d64  0700000a  beq 0x108b7d88
108b7d68  44109fe5  ldr r1, [0x108b7db4]
108b7d6c  070050e3  cmp r0, #0x7
108b7d70  0600a023  movcs r0, #0x6
108b7d74  800081e0  add r0, r1, r0, lsl #0x1
108b7d78  b000d0e1  ldrh r0, [r0,#0x0]
108b7d7c  8e52feeb  bl 0x1084c7bc
108b7d80  000050e3  cmp r0, #0x0
108b7d84  0500001a  bne 0x108b7da0
108b7d88  0500a0e1  cpy r0, r5
108b7d8c  0410a0e1  cpy r1, r4
108b7d90  d24efeeb  bl 0x1084b8e0
108b7d94  000050e3  cmp r0, #0x0
108b7d98  00f020e3  nop
108b7d9c  0300000a  beq 0x108b7db0
108b7da0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b7da4  1b10a0e3  mov r1, #0x1b
108b7da8  2300a0e3  mov r0, #0x23
108b7dac  c8b6feea  b 0x108658d4
108b7db0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
