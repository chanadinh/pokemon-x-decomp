108b7a88  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b7a8c  0150a0e1  cpy r5, r1
108b7a90  0240a0e1  cpy r4, r2
108b7a94  0200a0e3  mov r0, #0x2
108b7a98  9fcffeeb  bl 0x1086b91c
108b7a9c  040050e1  cmp r0, r4
108b7aa0  0c00001a  bne 0x108b7ad8
108b7aa4  0420a0e1  cpy r2, r4
108b7aa8  0310a0e3  mov r1, #0x3
108b7aac  0500a0e1  cpy r0, r5
108b7ab0  31c9feeb  bl 0x10869f7c
108b7ab4  0040a0e1  cpy r4, r0
108b7ab8  7d20a0e3  mov r2, #0x7d
108b7abc  0110a0e3  mov r1, #0x1
108b7ac0  040080e2  add r0, r0, #0x4
108b7ac4  0256feeb  bl 0x1084d2d4
108b7ac8  0410a0e1  cpy r1, r4
108b7acc  0500a0e1  cpy r0, r5
108b7ad0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b7ad4  1bc0feea  b 0x10867b48
108b7ad8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
