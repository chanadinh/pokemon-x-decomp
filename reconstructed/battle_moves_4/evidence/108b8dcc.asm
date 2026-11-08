108b8dcc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b8dd0  0050a0e1  cpy r5, r0
108b8dd4  0240a0e1  cpy r4, r2
108b8dd8  0200a0e3  mov r0, #0x2
108b8ddc  cecafeeb  bl 0x1086b91c
108b8de0  040050e1  cmp r0, r4
108b8de4  0200001a  bne 0x108b8df4
108b8de8  0500a0e1  cpy r0, r5
108b8dec  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b8df0  f550feea  b 0x1084d1cc
108b8df4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
