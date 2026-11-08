108b76c8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b76cc  0150a0e1  cpy r5, r1
108b76d0  0240a0e1  cpy r4, r2
108b76d4  0300a0e3  mov r0, #0x3
108b76d8  8fd0feeb  bl 0x1086b91c
108b76dc  040050e1  cmp r0, r4
108b76e0  0700001a  bne 0x108b7704
108b76e4  0500a0e1  cpy r0, r5
108b76e8  4c52feeb  bl 0x1084c020
108b76ec  000050e3  cmp r0, #0x0
108b76f0  0300000a  beq 0x108b7704
108b76f4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b76f8  0110a0e3  mov r1, #0x1
108b76fc  5300a0e3  mov r0, #0x53
108b7700  73b8feea  b 0x108658d4
108b7704  7080bde8  ldmia sp!,{r4,r5,r6,pc}
