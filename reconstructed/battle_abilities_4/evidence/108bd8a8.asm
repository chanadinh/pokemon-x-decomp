108bd8a8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bd8ac  0150a0e1  cpy r5, r1
108bd8b0  0240a0e1  cpy r4, r2
108bd8b4  0200a0e3  mov r0, #0x2
108bd8b8  17b8feeb  bl 0x1086b91c
108bd8bc  040050e1  cmp r0, r4
108bd8c0  0c00001a  bne 0x108bd8f8
108bd8c4  0410a0e1  cpy r1, r4
108bd8c8  0500a0e1  cpy r0, r5
108bd8cc  f35afeeb  bl 0x108544a0
108bd8d0  f5bffeeb  bl 0x1086d8ac
108bd8d4  000050e3  cmp r0, #0x0
108bd8d8  0600000a  beq 0x108bd8f8
108bd8dc  061ba0e3  mov r1, #0x1800
108bd8e0  3600a0e3  mov r0, #0x36
108bd8e4  663dfeeb  bl 0x1084ce84
108bd8e8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bd8ec  0010a0e3  mov r1, #0x0
108bd8f0  5300a0e3  mov r0, #0x53
108bd8f4  f69ffeea  b 0x108658d4
108bd8f8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
