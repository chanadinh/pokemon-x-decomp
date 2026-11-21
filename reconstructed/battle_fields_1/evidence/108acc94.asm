108acc94  70402de9  stmdb sp!,{r4,r5,r6,lr}
108acc98  0250a0e1  cpy r5, r2
108acc9c  0360a0e1  cpy r6, r3
108acca0  0400a0e3  mov r0, #0x4
108acca4  1cfbfeeb  bl 0x1086b91c
108acca8  ff4000e2  and r4, r0, #0xff
108accac  0400a0e1  cpy r0, r4
108accb0  c27cfeeb  bl 0x1084bfc0
108accb4  050050e1  cmp r0, r5
108accb8  1200001a  bne 0x108acd08
108accbc  0300a0e3  mov r0, #0x3
108accc0  15fbfeeb  bl 0x1086b91c
108accc4  040050e1  cmp r0, r4
108accc8  0e00000a  beq 0x108acd08
108acccc  1e00a0e3  mov r0, #0x1e
108accd0  11fbfeeb  bl 0x1086b91c
108accd4  7040ffe6  uxth r4, r0
108accd8  0400a0e1  cpy r0, r4
108accdc  887cfeeb  bl 0x1084bf04
108acce0  000050e3  cmp r0, #0x0
108acce4  0300001a  bne 0x108accf8
108acce8  060054e3  cmp r4, #0x6
108accec  0100000a  beq 0x108accf8
108accf0  0e0054e3  cmp r4, #0xe
108accf4  0300001a  bne 0x108acd08
108accf8  0110a0e3  mov r1, #0x1
108accfc  4200a0e3  mov r0, #0x42
108acd00  f3e2feeb  bl 0x108658d4
108acd04  000086e5  str r0, [r6,#0x0]
108acd08  7080bde8  ldmia sp!,{r4,r5,r6,pc}
