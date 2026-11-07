108b59a8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b59ac  0150a0e1  cpy r5, r1
108b59b0  0240a0e1  cpy r4, r2
108b59b4  0300a0e3  mov r0, #0x3
108b59b8  d7d7feeb  bl 0x1086b91c
108b59bc  040050e1  cmp r0, r4
108b59c0  0900001a  bne 0x108b59ec
108b59c4  0410a0e1  cpy r1, r4
108b59c8  0500a0e1  cpy r0, r5
108b59cc  b37afeeb  bl 0x108544a0
108b59d0  0010a0e3  mov r1, #0x0
108b59d4  5458feeb  bl 0x1084bb2c
108b59d8  6410a0e3  mov r1, #0x64
108b59dc  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b59e0  800161e1  smulbb r1, r0, r1
108b59e4  3100a0e3  mov r0, #0x31
108b59e8  b9bffeea  b 0x108658d4
108b59ec  7080bde8  ldmia sp!,{r4,r5,r6,pc}
