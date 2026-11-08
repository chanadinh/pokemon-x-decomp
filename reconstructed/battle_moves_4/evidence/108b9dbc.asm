108b9dbc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b9dc0  0050a0e1  cpy r5, r0
108b9dc4  0240a0e1  cpy r4, r2
108b9dc8  0200a0e3  mov r0, #0x2
108b9dcc  d2c6feeb  bl 0x1086b91c
108b9dd0  040050e1  cmp r0, r4
108b9dd4  0200001a  bne 0x108b9de4
108b9dd8  0500a0e1  cpy r0, r5
108b9ddc  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b9de0  f94cfeea  b 0x1084d1cc
108b9de4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
