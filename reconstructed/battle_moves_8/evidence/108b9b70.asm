108b9b70  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b9b74  0150a0e1  cpy r5, r1
108b9b78  0240a0e1  cpy r4, r2
108b9b7c  0300a0e3  mov r0, #0x3
108b9b80  65c7feeb  bl 0x1086b91c
108b9b84  040050e1  cmp r0, r4
108b9b88  1200001a  bne 0x108b9bd8
108b9b8c  0400a0e3  mov r0, #0x4
108b9b90  61c7feeb  bl 0x1086b91c
108b9b94  ff1000e2  and r1, r0, #0xff
108b9b98  0500a0e1  cpy r0, r5
108b9b9c  3f6afeeb  bl 0x108544a0
108b9ba0  0040a0e1  cpy r4, r0
108b9ba4  1110a0e3  mov r1, #0x11
108b9ba8  1ceafeeb  bl 0x10874420
108b9bac  360050e3  cmp r0, #0x36
108b9bb0  0400000a  beq 0x108b9bc8
108b9bb4  1110a0e3  mov r1, #0x11
108b9bb8  0400a0e1  cpy r0, r4
108b9bbc  17eafeeb  bl 0x10874420
108b9bc0  0f0050e3  cmp r0, #0xf
108b9bc4  0300001a  bne 0x108b9bd8
108b9bc8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b9bcc  0110a0e3  mov r1, #0x1
108b9bd0  4100a0e3  mov r0, #0x41
108b9bd4  3eaffeea  b 0x108658d4
108b9bd8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
