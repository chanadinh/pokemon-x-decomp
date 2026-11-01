108bd2dc  10402de9  stmdb sp!,{r4,lr}
108bd2e0  0240a0e1  cpy r4, r2
108bd2e4  0300a0e3  mov r0, #0x3
108bd2e8  8bb9feeb  bl 0x1086b91c
108bd2ec  040050e1  cmp r0, r4
108bd2f0  0f00001a  bne 0x108bd334
108bd2f4  1200a0e3  mov r0, #0x12
108bd2f8  87b9feeb  bl 0x1086b91c
108bd2fc  7040ffe6  uxth r4, r0
108bd300  1e10a0e3  mov r1, #0x1e
108bd304  0400a0e1  cpy r0, r4
108bd308  610cfdeb  bl 0x10800494
108bd30c  000050e3  cmp r0, #0x0
108bd310  0300001a  bne 0x108bd324
108bd314  1a0054e3  cmp r4, #0x1a
108bd318  0100000a  beq 0x108bd324
108bd31c  880054e3  cmp r4, #0x88
108bd320  0300001a  bne 0x108bd334
108bd324  0c109fe5  ldr r1, [0x108bd338]
108bd328  1040bde8  ldmia sp!,{r4,lr}
108bd32c  3200a0e3  mov r0, #0x32
108bd330  d33efeea  b 0x1084ce84
108bd334  1080bde8  ldmia sp!,{r4,pc}
