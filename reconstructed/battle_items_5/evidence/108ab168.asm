108ab168  10402de9  stmdb sp!,{r4,lr}
108ab16c  0240a0e1  cpy r4, r2
108ab170  0200a0e3  mov r0, #0x2
108ab174  e801ffeb  bl 0x1086b91c
108ab178  040050e1  cmp r0, r4
108ab17c  0300001a  bne 0x108ab190
108ab180  1040bde8  ldmia sp!,{r4,lr}
108ab184  021ba0e3  mov r1, #0x800
108ab188  3600a0e3  mov r0, #0x36
108ab18c  3c87feea  b 0x1084ce84
108ab190  1080bde8  ldmia sp!,{r4,pc}
