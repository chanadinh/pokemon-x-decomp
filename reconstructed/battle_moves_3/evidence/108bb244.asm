108bb244  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bb248  0050a0e1  cpy r5, r0
108bb24c  0240a0e1  cpy r4, r2
108bb250  0300a0e3  mov r0, #0x3
108bb254  b0c1feeb  bl 0x1086b91c
108bb258  040050e1  cmp r0, r4
108bb25c  0200001a  bne 0x108bb26c
108bb260  0500a0e1  cpy r0, r5
108bb264  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bb268  1f3ffeea  b 0x1084aeec
108bb26c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
