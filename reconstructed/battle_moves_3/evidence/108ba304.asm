108ba304  70402de9  stmdb sp!,{r4,r5,r6,lr}
108ba308  0050a0e1  cpy r5, r0
108ba30c  0240a0e1  cpy r4, r2
108ba310  0300a0e3  mov r0, #0x3
108ba314  80c5feeb  bl 0x1086b91c
108ba318  040050e1  cmp r0, r4
108ba31c  0300001a  bne 0x108ba330
108ba320  0500a0e1  cpy r0, r5
108ba324  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108ba328  04109fe5  ldr r1, [0x108ba334]
108ba32c  f142feea  b 0x1084aef8
108ba330  7080bde8  ldmia sp!,{r4,r5,r6,pc}
