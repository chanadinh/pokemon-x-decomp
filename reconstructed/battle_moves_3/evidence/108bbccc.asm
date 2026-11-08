108bbccc  10402de9  stmdb sp!,{r4,lr}
108bbcd0  0240a0e1  cpy r4, r2
108bbcd4  0300a0e3  mov r0, #0x3
108bbcd8  0fbffeeb  bl 0x1086b91c
108bbcdc  040050e1  cmp r0, r4
108bbce0  0300001a  bne 0x108bbcf4
108bbce4  1040bde8  ldmia sp!,{r4,lr}
108bbce8  0110a0e3  mov r1, #0x1
108bbcec  4200a0e3  mov r0, #0x42
108bbcf0  f7a6feea  b 0x108658d4
108bbcf4  1080bde8  ldmia sp!,{r4,pc}
