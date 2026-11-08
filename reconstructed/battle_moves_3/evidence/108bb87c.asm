108bb87c  10402de9  stmdb sp!,{r4,lr}
108bb880  0240a0e1  cpy r4, r2
108bb884  0300a0e3  mov r0, #0x3
108bb888  23c0feeb  bl 0x1086b91c
108bb88c  040050e1  cmp r0, r4
108bb890  0300001a  bne 0x108bb8a4
108bb894  1040bde8  ldmia sp!,{r4,lr}
108bb898  0110a0e3  mov r1, #0x1
108bb89c  5300a0e3  mov r0, #0x53
108bb8a0  0ba8feea  b 0x108658d4
108bb8a4  1080bde8  ldmia sp!,{r4,pc}
