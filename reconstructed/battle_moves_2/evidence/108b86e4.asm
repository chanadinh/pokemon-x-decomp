108b86e4  10402de9  stmdb sp!,{r4,lr}
108b86e8  0340a0e1  cpy r4, r3
108b86ec  0300a0e3  mov r0, #0x3
108b86f0  89ccfeeb  bl 0x1086b91c
108b86f4  001094e5  ldr r1, [r4,#0x0]
108b86f8  010050e1  cmp r0, r1
108b86fc  0300001a  bne 0x108b8710
108b8700  1040bde8  ldmia sp!,{r4,lr}
108b8704  061ba0e3  mov r1, #0x1800
108b8708  3200a0e3  mov r0, #0x32
108b870c  dc51feea  b 0x1084ce84
108b8710  1080bde8  ldmia sp!,{r4,pc}
