108afe50  70402de9  stmdb sp!,{r4,r5,r6,lr}
108afe54  0150a0e1  cpy r5, r1
108afe58  0240a0e1  cpy r4, r2
108afe5c  0300a0e3  mov r0, #0x3
108afe60  adeefeeb  bl 0x1086b91c
108afe64  040050e1  cmp r0, r4
108afe68  0c00001a  bne 0x108afea0
108afe6c  0400a0e3  mov r0, #0x4
108afe70  a9eefeeb  bl 0x1086b91c
108afe74  ff1000e2  and r1, r0, #0xff
108afe78  0500a0e1  cpy r0, r5
108afe7c  8791feeb  bl 0x108544a0
108afe80  0210a0e3  mov r1, #0x2
108afe84  ff9afeeb  bl 0x10856a88
108afe88  000050e3  cmp r0, #0x0
108afe8c  0300001a  bne 0x108afea0
108afe90  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108afe94  0110a0e3  mov r1, #0x1
108afe98  4100a0e3  mov r0, #0x41
108afe9c  8cd6feea  b 0x108658d4
108afea0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
