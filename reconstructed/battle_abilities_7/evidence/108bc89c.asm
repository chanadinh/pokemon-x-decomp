108bc89c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bc8a0  0240a0e1  cpy r4, r2
108bc8a4  0350a0e1  cpy r5, r3
108bc8a8  0200a0e3  mov r0, #0x2
108bc8ac  1abcfeeb  bl 0x1086b91c
108bc8b0  040050e1  cmp r0, r4
108bc8b4  0700001a  bne 0x108bc8d8
108bc8b8  5300a0e3  mov r0, #0x53
108bc8bc  16bcfeeb  bl 0x1086b91c
108bc8c0  000050e3  cmp r0, #0x0
108bc8c4  0300001a  bne 0x108bc8d8
108bc8c8  0110a0e3  mov r1, #0x1
108bc8cc  5300a0e3  mov r0, #0x53
108bc8d0  ffa3feeb  bl 0x108658d4
108bc8d4  000085e5  str r0, [r5,#0x0]
108bc8d8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
