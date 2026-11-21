1084bb2c  70402de9  stmdb sp!,{r4,r5,r6,lr}
1084bb30  050051e3  cmp r1, #0x5
1084bb34  0050a0e1  cpy r5, r0
1084bb38  0140a0e1  cpy r4, r1
1084bb3c  0300003a  bcc 0x1084bb50
1084bb40  0020a0e3  mov r2, #0x0
1084bb44  0210a0e1  cpy r1, r2
1084bb48  0200a0e1  cpy r0, r2
1084bb4c  10d2feeb  bl 0x10800394
1084bb50  040085e0  add r0, r5, r4
1084bb54  6d01d0e5  ldrb r0, [r0,#0x16d]
1084bb58  7080bde8  ldmia sp!,{r4,r5,r6,pc}
