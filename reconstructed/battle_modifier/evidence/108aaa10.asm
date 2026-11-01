108aaa10  70402de9  stmdb sp!,{r4,r5,r6,lr}
108aaa14  0050a0e1  cpy r5, r0
108aaa18  0240a0e1  cpy r4, r2
108aaa1c  0300a0e3  mov r0, #0x3
108aaa20  bd03ffeb  bl 0x1086b91c
108aaa24  040050e1  cmp r0, r4
108aaa28  0500001a  bne 0x108aaa44
108aaa2c  0500a0e1  cpy r0, r5
108aaa30  6b88feeb  bl 0x1084cbe4
108aaa34  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108aaa38  0010a0e1  cpy r1, r0
108aaa3c  3600a0e3  mov r0, #0x36
108aaa40  0f89feea  b 0x1084ce84
108aaa44  7080bde8  ldmia sp!,{r4,r5,r6,pc}
