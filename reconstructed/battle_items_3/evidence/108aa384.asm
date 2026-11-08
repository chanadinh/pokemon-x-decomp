108aa384  70402de9  stmdb sp!,{r4,r5,r6,lr}
108aa388  0050a0e1  cpy r5, r0
108aa38c  0240a0e1  cpy r4, r2
108aa390  0300a0e3  mov r0, #0x3
108aa394  6005ffeb  bl 0x1086b91c
108aa398  040050e1  cmp r0, r4
108aa39c  0b00001a  bne 0x108aa3d0
108aa3a0  1200a0e3  mov r0, #0x12
108aa3a4  5c05ffeb  bl 0x1086b91c
108aa3a8  7000ffe6  uxth r0, r0
108aa3ac  0a59fdeb  bl 0x108007dc
108aa3b0  010050e3  cmp r0, #0x1
108aa3b4  0500001a  bne 0x108aa3d0
108aa3b8  0500a0e1  cpy r0, r5
108aa3bc  088afeeb  bl 0x1084cbe4
108aa3c0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108aa3c4  0010a0e1  cpy r1, r0
108aa3c8  3200a0e3  mov r0, #0x32
108aa3cc  ac8afeea  b 0x1084ce84
108aa3d0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
