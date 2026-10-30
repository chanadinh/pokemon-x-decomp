1086b9a4  10402de9  stmdb sp!,{r4,lr}
1086b9a8  48409fe5  ldr r4, [0x1086b9f8]
1086b9ac  0720a0e3  mov r2, #0x7
1086b9b0  0130a0e3  mov r3, #0x1
1086b9b4  68c094e5  ldr r12, [r4,#0x68]
1086b9b8  01c08ce2  add r12, r12, #0x1
1086b9bc  68c084e5  str r12, [r4,#0x68]
1086b9c0  34e5ffeb  bl 0x10864e98
1086b9c4  680094e5  ldr r0, [r4,#0x68]
1086b9c8  010050e2  subs r0, r0, #0x1
1086b9cc  680084e5  str r0, [r4,#0x68]
1086b9d0  0700001a  bne 0x1086b9f4
1086b9d4  600094e5  ldr r0, [r4,#0x60]
1086b9d8  000050e3  cmp r0, #0x0
1086b9dc  0010a013  movne r1, #0x0
1086b9e0  0300000a  beq 0x1086b9f4
1086b9e4  b811c0e1  strh r1, [r0,#0x18]
1086b9e8  040090e5  ldr r0, [r0,#0x4]
1086b9ec  000050e3  cmp r0, #0x0
1086b9f0  fbffff1a  bne 0x1086b9e4
1086b9f4  1080bde8  ldmia sp!,{r4,pc}
