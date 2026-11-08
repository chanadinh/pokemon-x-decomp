108a63ac  10402de9  stmdb sp!,{r4,lr}
108a63b0  0240a0e1  cpy r4, r2
108a63b4  0300a0e3  mov r0, #0x3
108a63b8  5715ffeb  bl 0x1086b91c
108a63bc  040050e1  cmp r0, r4
108a63c0  0600001a  bne 0x108a63e0
108a63c4  2d00a0e3  mov r0, #0x2d
108a63c8  5315ffeb  bl 0x1086b91c
108a63cc  010080e2  add r0, r0, #0x1
108a63d0  ff1000e2  and r1, r0, #0xff
108a63d4  1040bde8  ldmia sp!,{r4,lr}
108a63d8  2d00a0e3  mov r0, #0x2d
108a63dc  3cfdfeea  b 0x108658d4
108a63e0  1080bde8  ldmia sp!,{r4,pc}
