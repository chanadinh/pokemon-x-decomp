108b97c0  10402de9  stmdb sp!,{r4,lr}
108b97c4  0240a0e1  cpy r4, r2
108b97c8  0300a0e3  mov r0, #0x3
108b97cc  52c8feeb  bl 0x1086b91c
108b97d0  040050e1  cmp r0, r4
108b97d4  0700001a  bne 0x108b97f8
108b97d8  2200a0e3  mov r0, #0x22
108b97dc  4ec8feeb  bl 0x1086b91c
108b97e0  030050e3  cmp r0, #0x3
108b97e4  0300001a  bne 0x108b97f8
108b97e8  1040bde8  ldmia sp!,{r4,lr}
108b97ec  0010a0e3  mov r1, #0x0
108b97f0  4300a0e3  mov r0, #0x43
108b97f4  36b0feea  b 0x108658d4
108b97f8  1080bde8  ldmia sp!,{r4,pc}
