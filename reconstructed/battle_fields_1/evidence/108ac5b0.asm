108ac5b0  10402de9  stmdb sp!,{r4,lr}
108ac5b4  0240a0e1  cpy r4, r2
108ac5b8  0300a0e3  mov r0, #0x3
108ac5bc  d6fcfeeb  bl 0x1086b91c
108ac5c0  ff0000e2  and r0, r0, #0xff
108ac5c4  7d7efeeb  bl 0x1084bfc0
108ac5c8  040050e1  cmp r0, r4
108ac5cc  0300001a  bne 0x108ac5e0
108ac5d0  1040bde8  ldmia sp!,{r4,lr}
108ac5d4  0110a0e3  mov r1, #0x1
108ac5d8  4600a0e3  mov r0, #0x46
108ac5dc  bce4feea  b 0x108658d4
108ac5e0  1080bde8  ldmia sp!,{r4,pc}
