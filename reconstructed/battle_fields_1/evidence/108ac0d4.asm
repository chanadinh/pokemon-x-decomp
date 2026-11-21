108ac0d4  10402de9  stmdb sp!,{r4,lr}
108ac0d8  0240a0e1  cpy r4, r2
108ac0dc  0200a0e3  mov r0, #0x2
108ac0e0  0dfefeeb  bl 0x1086b91c
108ac0e4  ff0000e2  and r0, r0, #0xff
108ac0e8  b47ffeeb  bl 0x1084bfc0
108ac0ec  040050e1  cmp r0, r4
108ac0f0  0300001a  bne 0x108ac104
108ac0f4  1040bde8  ldmia sp!,{r4,lr}
108ac0f8  011ba0e3  mov r1, #0x400
108ac0fc  3600a0e3  mov r0, #0x36
108ac100  5f83feea  b 0x1084ce84
108ac104  1080bde8  ldmia sp!,{r4,pc}
