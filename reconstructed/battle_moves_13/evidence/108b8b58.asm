108b8b58  10402de9  stmdb sp!,{r4,lr}
108b8b5c  0240a0e1  cpy r4, r2
108b8b60  0300a0e3  mov r0, #0x3
108b8b64  6ccbfeeb  bl 0x1086b91c
108b8b68  040050e1  cmp r0, r4
108b8b6c  0300001a  bne 0x108b8b80
108b8b70  1040bde8  ldmia sp!,{r4,lr}
108b8b74  0110a0e3  mov r1, #0x1
108b8b78  5300a0e3  mov r0, #0x53
108b8b7c  54b3feea  b 0x108658d4
108b8b80  1080bde8  ldmia sp!,{r4,pc}
