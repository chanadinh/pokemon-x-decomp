108be4ec  10402de9  stmdb sp!,{r4,lr}
108be4f0  0240a0e1  cpy r4, r2
108be4f4  0300a0e3  mov r0, #0x3
108be4f8  07b5feeb  bl 0x1086b91c
108be4fc  040050e1  cmp r0, r4
108be500  0300001a  bne 0x108be514
108be504  1040bde8  ldmia sp!,{r4,lr}
108be508  0110a0e3  mov r1, #0x1
108be50c  4200a0e3  mov r0, #0x42
108be510  ef9cfeea  b 0x108658d4
108be514  1080bde8  ldmia sp!,{r4,pc}
