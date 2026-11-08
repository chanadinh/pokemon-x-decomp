108bab20  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bab24  0150a0e1  cpy r5, r1
108bab28  0240a0e1  cpy r4, r2
108bab2c  0200a0e3  mov r0, #0x2
108bab30  79c3feeb  bl 0x1086b91c
108bab34  040050e1  cmp r0, r4
108bab38  0700001a  bne 0x108bab5c
108bab3c  0020a0e3  mov r2, #0x0
108bab40  0210a0e1  cpy r1, r2
108bab44  0500a0e1  cpy r0, r5
108bab48  570300eb  bl 0x108bb8ac
108bab4c  0010a0e1  cpy r1, r0
108bab50  0500a0e1  cpy r0, r5
108bab54  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bab58  e644feea  b 0x1084bef8
108bab5c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
