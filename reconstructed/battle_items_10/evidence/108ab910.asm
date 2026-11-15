108ab910  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108ab914  0060a0e1  cpy r6, r0
108ab918  0150a0e1  cpy r5, r1
108ab91c  0240a0e1  cpy r4, r2
108ab920  0370a0e1  cpy r7, r3
108ab924  0300a0e3  mov r0, #0x3
108ab928  fbfffeeb  bl 0x1086b91c
108ab92c  040050e1  cmp r0, r4
108ab930  0900001a  bne 0x108ab95c
108ab934  000097e5  ldr r0, [r7,#0x0]
108ab938  000050e3  cmp r0, #0x0
108ab93c  0600000a  beq 0x108ab95c
108ab940  0600a0e1  cpy r0, r6
108ab944  0884feeb  bl 0x1084c96c
108ab948  0420a0e1  cpy r2, r4
108ab94c  0500a0e1  cpy r0, r5
108ab950  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108ab954  0010a0e3  mov r1, #0x0
108ab958  fa83feea  b 0x1084c948
108ab95c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
