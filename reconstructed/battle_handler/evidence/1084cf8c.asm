1084cf8c  70402de9  stmdb sp!,{r4,r5,r6,lr}
1084cf90  070051e3  cmp r1, #0x7
1084cf94  0050a0e1  cpy r5, r0
1084cf98  0140a0e1  cpy r4, r1
1084cf9c  0260a0e1  cpy r6, r2
1084cfa0  0300003a  bcc 0x1084cfb4
1084cfa4  0020a0e3  mov r2, #0x0
1084cfa8  0210a0e1  cpy r1, r2
1084cfac  0200a0e1  cpy r0, r2
1084cfb0  f7ccfeeb  bl 0x10800394
1084cfb4  040185e0  add r0, r5, r4, lsl #0x2
1084cfb8  1c6080e5  str r6, [r0,#0x1c]
1084cfbc  7080bde8  ldmia sp!,{r4,r5,r6,pc}
