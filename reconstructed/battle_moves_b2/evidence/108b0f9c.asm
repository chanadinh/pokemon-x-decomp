108b0f9c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b0fa0  0060a0e1  cpy r6, r0
108b0fa4  0150a0e1  cpy r5, r1
108b0fa8  0240a0e1  cpy r4, r2
108b0fac  0300a0e3  mov r0, #0x3
108b0fb0  59eafeeb  bl 0x1086b91c
108b0fb4  040050e1  cmp r0, r4
108b0fb8  0600001a  bne 0x108b0fd8
108b0fbc  0600a0e1  cpy r0, r6
108b0fc0  696efeeb  bl 0x1084c96c
108b0fc4  0420a0e1  cpy r2, r4
108b0fc8  0500a0e1  cpy r0, r5
108b0fcc  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b0fd0  2910a0e3  mov r1, #0x29
108b0fd4  5b6efeea  b 0x1084c948
108b0fd8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
