108b9b44  10402de9  stmdb sp!,{r4,lr}
108b9b48  0240a0e1  cpy r4, r2
108b9b4c  0300a0e3  mov r0, #0x3
108b9b50  71c7feeb  bl 0x1086b91c
108b9b54  040050e1  cmp r0, r4
108b9b58  0300001a  bne 0x108b9b6c
108b9b5c  1040bde8  ldmia sp!,{r4,lr}
108b9b60  0110a0e3  mov r1, #0x1
108b9b64  5300a0e3  mov r0, #0x53
108b9b68  59affeea  b 0x108658d4
108b9b6c  1080bde8  ldmia sp!,{r4,pc}
