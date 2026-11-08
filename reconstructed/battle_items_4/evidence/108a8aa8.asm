108a8aa8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a8aac  0350a0e1  cpy r5, r3
108a8ab0  0160a0e1  cpy r6, r1
108a8ab4  0240a0e1  cpy r4, r2
108a8ab8  0300a0e3  mov r0, #0x3
108a8abc  960bffeb  bl 0x1086b91c
108a8ac0  040050e1  cmp r0, r4
108a8ac4  0e00001a  bne 0x108a8b04
108a8ac8  1600a0e3  mov r0, #0x16
108a8acc  920bffeb  bl 0x1086b91c
108a8ad0  110050e3  cmp r0, #0x11
108a8ad4  0a00001a  bne 0x108a8b04
108a8ad8  000095e5  ldr r0, [r5,#0x0]
108a8adc  010050e3  cmp r0, #0x1
108a8ae0  0300000a  beq 0x108a8af4
108a8ae4  0600a0e1  cpy r0, r6
108a8ae8  078ffeeb  bl 0x1084c70c
108a8aec  000050e3  cmp r0, #0x0
108a8af0  0300000a  beq 0x108a8b04
108a8af4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a8af8  3200a0e3  mov r0, #0x32
108a8afc  04109fe5  ldr r1, [0x108a8b08]
108a8b00  df90feea  b 0x1084ce84
108a8b04  7080bde8  ldmia sp!,{r4,r5,r6,pc}
