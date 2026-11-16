108b9ab8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b9abc  0050a0e1  cpy r5, r0
108b9ac0  0240a0e1  cpy r4, r2
108b9ac4  0200a0e3  mov r0, #0x2
108b9ac8  93c7feeb  bl 0x1086b91c
108b9acc  040050e1  cmp r0, r4
108b9ad0  0600001a  bne 0x108b9af0
108b9ad4  0c00a0e3  mov r0, #0xc
108b9ad8  8fc7feeb  bl 0x1086b91c
108b9adc  080050e3  cmp r0, #0x8
108b9ae0  0200000a  beq 0x108b9af0
108b9ae4  0500a0e1  cpy r0, r5
108b9ae8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b9aec  b64dfeea  b 0x1084d1cc
108b9af0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
