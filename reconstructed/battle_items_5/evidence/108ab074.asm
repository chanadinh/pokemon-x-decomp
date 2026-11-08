108ab074  70402de9  stmdb sp!,{r4,r5,r6,lr}
108ab078  0150a0e1  cpy r5, r1
108ab07c  0240a0e1  cpy r4, r2
108ab080  0400a0e3  mov r0, #0x4
108ab084  2402ffeb  bl 0x1086b91c
108ab088  040050e1  cmp r0, r4
108ab08c  0f00001a  bne 0x108ab0d0
108ab090  0410a0e1  cpy r1, r4
108ab094  0500a0e1  cpy r0, r5
108ab098  00a5feeb  bl 0x108544a0
108ab09c  bc00d0e1  ldrh r0, [r0,#0xc]
108ab0a0  5f0f50e3  cmp r0, #0x17c
108ab0a4  011c4012  subne r1, r0, #0x100
108ab0a8  7d105112  subnes r1, r1, #0x7d
108ab0ac  0700001a  bne 0x108ab0d0
108ab0b0  1b00a0e3  mov r0, #0x1b
108ab0b4  1802ffeb  bl 0x1086b91c
108ab0b8  020050e3  cmp r0, #0x2
108ab0bc  0300001a  bne 0x108ab0d0
108ab0c0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108ab0c4  061ba0e3  mov r1, #0x1800
108ab0c8  3600a0e3  mov r0, #0x36
108ab0cc  6c87feea  b 0x1084ce84
108ab0d0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
