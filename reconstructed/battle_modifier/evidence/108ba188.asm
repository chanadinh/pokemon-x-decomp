108ba188  70402de9  stmdb sp!,{r4,r5,r6,lr}
108ba18c  0150a0e1  cpy r5, r1
108ba190  0240a0e1  cpy r4, r2
108ba194  0300a0e3  mov r0, #0x3
108ba198  dfc5feeb  bl 0x1086b91c
108ba19c  040050e1  cmp r0, r4
108ba1a0  0c00001a  bne 0x108ba1d8
108ba1a4  0400a0e3  mov r0, #0x4
108ba1a8  dbc5feeb  bl 0x1086b91c
108ba1ac  ff1000e2  and r1, r0, #0xff
108ba1b0  0500a0e1  cpy r0, r5
108ba1b4  b968feeb  bl 0x108544a0
108ba1b8  0810a0e3  mov r1, #0x8
108ba1bc  224afeeb  bl 0x1084ca4c
108ba1c0  000050e3  cmp r0, #0x0
108ba1c4  0300000a  beq 0x108ba1d8
108ba1c8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108ba1cc  021aa0e3  mov r1, #0x2000
108ba1d0  3600a0e3  mov r0, #0x36
108ba1d4  2a4bfeea  b 0x1084ce84
108ba1d8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
