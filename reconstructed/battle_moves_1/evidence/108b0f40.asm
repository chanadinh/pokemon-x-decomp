108b0f40  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b0f44  0150a0e1  cpy r5, r1
108b0f48  0240a0e1  cpy r4, r2
108b0f4c  0300a0e3  mov r0, #0x3
108b0f50  71eafeeb  bl 0x1086b91c
108b0f54  040050e1  cmp r0, r4
108b0f58  0e00001a  bne 0x108b0f98
108b0f5c  0400a0e3  mov r0, #0x4
108b0f60  6deafeeb  bl 0x1086b91c
108b0f64  ff1000e2  and r1, r0, #0xff
108b0f68  0500a0e1  cpy r0, r5
108b0f6c  4b8dfeeb  bl 0x108544a0
108b0f70  0310a0e3  mov r1, #0x3
108b0f74  b46efeeb  bl 0x1084ca4c
108b0f78  000050e3  cmp r0, #0x0
108b0f7c  0500000a  beq 0x108b0f98
108b0f80  3100a0e3  mov r0, #0x31
108b0f84  64eafeeb  bl 0x1086b91c
108b0f88  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b0f8c  8010a0e1  mov r1, r0, lsl #0x1
108b0f90  3100a0e3  mov r0, #0x31
108b0f94  4ed2feea  b 0x108658d4
108b0f98  7080bde8  ldmia sp!,{r4,r5,r6,pc}
