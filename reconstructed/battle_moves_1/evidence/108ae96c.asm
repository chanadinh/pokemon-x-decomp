108ae96c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108ae970  0150a0e1  cpy r5, r1
108ae974  0240a0e1  cpy r4, r2
108ae978  0300a0e3  mov r0, #0x3
108ae97c  e6f3feeb  bl 0x1086b91c
108ae980  040050e1  cmp r0, r4
108ae984  0e00001a  bne 0x108ae9c4
108ae988  0400a0e3  mov r0, #0x4
108ae98c  e2f3feeb  bl 0x1086b91c
108ae990  ff1000e2  and r1, r0, #0xff
108ae994  0500a0e1  cpy r0, r5
108ae998  c096feeb  bl 0x108544a0
108ae99c  0110a0e3  mov r1, #0x1
108ae9a0  38a0feeb  bl 0x10856a88
108ae9a4  000050e3  cmp r0, #0x0
108ae9a8  0500000a  beq 0x108ae9c4
108ae9ac  3100a0e3  mov r0, #0x31
108ae9b0  d9f3feeb  bl 0x1086b91c
108ae9b4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108ae9b8  8010a0e1  mov r1, r0, lsl #0x1
108ae9bc  3100a0e3  mov r0, #0x31
108ae9c0  c3dbfeea  b 0x108658d4
108ae9c4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
