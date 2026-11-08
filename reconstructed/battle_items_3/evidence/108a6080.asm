108a6080  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a6084  0050a0e1  cpy r5, r0
108a6088  0240a0e1  cpy r4, r2
108a608c  0300a0e3  mov r0, #0x3
108a6090  2116ffeb  bl 0x1086b91c
108a6094  040050e1  cmp r0, r4
108a6098  0900001a  bne 0x108a60c4
108a609c  1600a0e3  mov r0, #0x16
108a60a0  1d16ffeb  bl 0x1086b91c
108a60a4  0c0050e3  cmp r0, #0xc
108a60a8  0500001a  bne 0x108a60c4
108a60ac  0500a0e1  cpy r0, r5
108a60b0  cb9afeeb  bl 0x1084cbe4
108a60b4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a60b8  0010a0e1  cpy r1, r0
108a60bc  3200a0e3  mov r0, #0x32
108a60c0  6f9bfeea  b 0x1084ce84
108a60c4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
