108b7b14  10402de9  stmdb sp!,{r4,lr}
108b7b18  0240a0e1  cpy r4, r2
108b7b1c  0300a0e3  mov r0, #0x3
108b7b20  7dcffeeb  bl 0x1086b91c
108b7b24  040050e1  cmp r0, r4
108b7b28  0700001a  bne 0x108b7b4c
108b7b2c  2200a0e3  mov r0, #0x22
108b7b30  79cffeeb  bl 0x1086b91c
108b7b34  050050e3  cmp r0, #0x5
108b7b38  0300001a  bne 0x108b7b4c
108b7b3c  1040bde8  ldmia sp!,{r4,lr}
108b7b40  0010a0e3  mov r1, #0x0
108b7b44  4300a0e3  mov r0, #0x43
108b7b48  61b7feea  b 0x108658d4
108b7b4c  1080bde8  ldmia sp!,{r4,pc}
