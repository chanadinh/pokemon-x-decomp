108b8d7c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b8d80  0150a0e1  cpy r5, r1
108b8d84  0240a0e1  cpy r4, r2
108b8d88  0300a0e3  mov r0, #0x3
108b8d8c  e2cafeeb  bl 0x1086b91c
108b8d90  040050e1  cmp r0, r4
108b8d94  0b00001a  bne 0x108b8dc8
108b8d98  0410a0e1  cpy r1, r4
108b8d9c  0500a0e1  cpy r0, r5
108b8da0  be6dfeeb  bl 0x108544a0
108b8da4  c0d2feeb  bl 0x1086d8ac
108b8da8  050050e3  cmp r0, #0x5
108b8dac  01005013  cmpne r0, #0x1
108b8db0  04005013  cmpne r0, #0x4
108b8db4  0300001a  bne 0x108b8dc8
108b8db8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b8dbc  021aa0e3  mov r1, #0x2000
108b8dc0  3200a0e3  mov r0, #0x32
108b8dc4  2e50feea  b 0x1084ce84
108b8dc8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
