108bf148  10402de9  stmdb sp!,{r4,lr}
108bf14c  0240a0e1  cpy r4, r2
108bf150  0300a0e3  mov r0, #0x3
108bf154  f0b1feeb  bl 0x1086b91c
108bf158  040050e1  cmp r0, r4
108bf15c  0900001a  bne 0x108bf188
108bf160  1200a0e3  mov r0, #0x12
108bf164  ecb1feeb  bl 0x1086b91c
108bf168  7000ffe6  uxth r0, r0
108bf16c  6895ffeb  bl 0x108a4714
108bf170  000050e3  cmp r0, #0x0
108bf174  0300000a  beq 0x108bf188
108bf178  1040bde8  ldmia sp!,{r4,lr}
108bf17c  061ba0e3  mov r1, #0x1800
108bf180  3200a0e3  mov r0, #0x32
108bf184  3e37feea  b 0x1084ce84
108bf188  1080bde8  ldmia sp!,{r4,pc}
