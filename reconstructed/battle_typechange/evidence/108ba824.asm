108ba824  10402de9  stmdb sp!,{r4,lr}
108ba828  0240a0e1  cpy r4, r2
108ba82c  0200a0e3  mov r0, #0x2
108ba830  39c4feeb  bl 0x1086b91c
108ba834  040050e1  cmp r0, r4
108ba838  0300001a  bne 0x108ba84c
108ba83c  1040bde8  ldmia sp!,{r4,lr}
108ba840  0110a0e3  mov r1, #0x1
108ba844  4c00a0e3  mov r0, #0x4c
108ba848  21acfeea  b 0x108658d4
108ba84c  1080bde8  ldmia sp!,{r4,pc}
