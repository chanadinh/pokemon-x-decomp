108a5db4  10402de9  stmdb sp!,{r4,lr}
108a5db8  0240a0e1  cpy r4, r2
108a5dbc  0200a0e3  mov r0, #0x2
108a5dc0  d516ffeb  bl 0x1086b91c
108a5dc4  040050e1  cmp r0, r4
108a5dc8  0300001a  bne 0x108a5ddc
108a5dcc  1040bde8  ldmia sp!,{r4,lr}
108a5dd0  021ba0e3  mov r1, #0x800
108a5dd4  3600a0e3  mov r0, #0x36
108a5dd8  299cfeea  b 0x1084ce84
108a5ddc  1080bde8  ldmia sp!,{r4,pc}
