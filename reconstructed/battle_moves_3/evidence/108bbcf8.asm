108bbcf8  10402de9  stmdb sp!,{r4,lr}
108bbcfc  0240a0e1  cpy r4, r2
108bbd00  0300a0e3  mov r0, #0x3
108bbd04  04bffeeb  bl 0x1086b91c
108bbd08  040050e1  cmp r0, r4
108bbd0c  0700001a  bne 0x108bbd30
108bbd10  1600a0e3  mov r0, #0x16
108bbd14  00bffeeb  bl 0x1086b91c
108bbd18  040050e3  cmp r0, #0x4
108bbd1c  0300001a  bne 0x108bbd30
108bbd20  1040bde8  ldmia sp!,{r4,lr}
108bbd24  0710a0e3  mov r1, #0x7
108bbd28  3900a0e3  mov r0, #0x39
108bbd2c  e8a6feea  b 0x108658d4
108bbd30  1080bde8  ldmia sp!,{r4,pc}
