108bbdf4  10402de9  stmdb sp!,{r4,lr}
108bbdf8  0240a0e1  cpy r4, r2
108bbdfc  0300a0e3  mov r0, #0x3
108bbe00  c5befeeb  bl 0x1086b91c
108bbe04  040050e1  cmp r0, r4
108bbe08  0300001a  bne 0x108bbe1c
108bbe0c  1040bde8  ldmia sp!,{r4,lr}
108bbe10  0110a0e3  mov r1, #0x1
108bbe14  5300a0e3  mov r0, #0x53
108bbe18  ada6feea  b 0x108658d4
108bbe1c  1080bde8  ldmia sp!,{r4,pc}
