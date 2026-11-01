108bf344  10402de9  stmdb sp!,{r4,lr}
108bf348  0240a0e1  cpy r4, r2
108bf34c  0300a0e3  mov r0, #0x3
108bf350  71b1feeb  bl 0x1086b91c
108bf354  040050e1  cmp r0, r4
108bf358  0a00001a  bne 0x108bf388
108bf35c  1200a0e3  mov r0, #0x12
108bf360  6db1feeb  bl 0x1086b91c
108bf364  7000ffe6  uxth r0, r0
108bf368  0010a0e3  mov r1, #0x0
108bf36c  4a04fdeb  bl 0x1080049c
108bf370  000050e3  cmp r0, #0x0
108bf374  0300000a  beq 0x108bf388
108bf378  0c109fe5  ldr r1, [0x108bf38c]
108bf37c  1040bde8  ldmia sp!,{r4,lr}
108bf380  3200a0e3  mov r0, #0x32
108bf384  be36feea  b 0x1084ce84
108bf388  1080bde8  ldmia sp!,{r4,pc}
