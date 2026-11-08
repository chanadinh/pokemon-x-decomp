108aa608  70402de9  stmdb sp!,{r4,r5,r6,lr}
108aa60c  0150a0e1  cpy r5, r1
108aa610  0240a0e1  cpy r4, r2
108aa614  0300a0e3  mov r0, #0x3
108aa618  bf04ffeb  bl 0x1086b91c
108aa61c  040050e1  cmp r0, r4
108aa620  0f00001a  bne 0x108aa664
108aa624  0410a0e1  cpy r1, r4
108aa628  0500a0e1  cpy r0, r5
108aa62c  9ba7feeb  bl 0x108544a0
108aa630  bc00d0e1  ldrh r0, [r0,#0xc]
108aa634  5f0f50e3  cmp r0, #0x17c
108aa638  011c4012  subne r1, r0, #0x100
108aa63c  7d105112  subnes r1, r1, #0x7d
108aa640  0700001a  bne 0x108aa664
108aa644  1b00a0e3  mov r0, #0x1b
108aa648  b304ffeb  bl 0x1086b91c
108aa64c  020050e3  cmp r0, #0x2
108aa650  0300001a  bne 0x108aa664
108aa654  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108aa658  061ba0e3  mov r1, #0x1800
108aa65c  3600a0e3  mov r0, #0x36
108aa660  078afeea  b 0x1084ce84
108aa664  7080bde8  ldmia sp!,{r4,r5,r6,pc}
