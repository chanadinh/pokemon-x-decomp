108bfed8  10402de9  stmdb sp!,{r4,lr}
108bfedc  0240a0e1  cpy r4, r2
108bfee0  0200a0e3  mov r0, #0x2
108bfee4  8caefeeb  bl 0x1086b91c
108bfee8  040050e1  cmp r0, r4
108bfeec  0300001a  bne 0x108bff00
108bfef0  1040bde8  ldmia sp!,{r4,lr}
108bfef4  021ba0e3  mov r1, #0x800
108bfef8  3600a0e3  mov r0, #0x36
108bfefc  e033feea  b 0x1084ce84
108bff00  1080bde8  ldmia sp!,{r4,pc}
