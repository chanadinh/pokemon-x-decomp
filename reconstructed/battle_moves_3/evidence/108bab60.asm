108bab60  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bab64  0150a0e1  cpy r5, r1
108bab68  0240a0e1  cpy r4, r2
108bab6c  0300a0e3  mov r0, #0x3
108bab70  69c3feeb  bl 0x1086b91c
108bab74  040050e1  cmp r0, r4
108bab78  0800001a  bne 0x108baba0
108bab7c  0410a0e1  cpy r1, r4
108bab80  0500a0e1  cpy r0, r5
108bab84  4566feeb  bl 0x108544a0
108bab88  0d10a0e3  mov r1, #0xd
108bab8c  23e6feeb  bl 0x10874420
108bab90  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bab94  0010a0e1  cpy r1, r0
108bab98  3800a0e3  mov r0, #0x38
108bab9c  4cabfeea  b 0x108658d4
108baba0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
