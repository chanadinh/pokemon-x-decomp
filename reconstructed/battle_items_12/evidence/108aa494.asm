108aa494  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108aa498  0060a0e1  cpy r6, r0
108aa49c  0150a0e1  cpy r5, r1
108aa4a0  0240a0e1  cpy r4, r2
108aa4a4  0200a0e3  mov r0, #0x2
108aa4a8  1b05ffeb  bl 0x1086b91c
108aa4ac  040050e1  cmp r0, r4
108aa4b0  1500001a  bne 0x108aa50c
108aa4b4  0500a0e1  cpy r0, r5
108aa4b8  0410a0e1  cpy r1, r4
108aa4bc  f7a7feeb  bl 0x108544a0
108aa4c0  0070a0e1  cpy r7, r0
108aa4c4  6b87feeb  bl 0x1084c278
108aa4c8  0010a0e1  cpy r1, r0
108aa4cc  0700a0e1  cpy r0, r7
108aa4d0  5387feeb  bl 0x1084c224
108aa4d4  040050e3  cmp r0, #0x4
108aa4d8  0010a0e1  cpy r1, r0
108aa4dc  0a00000a  beq 0x108aa50c
108aa4e0  0700a0e1  cpy r0, r7
108aa4e4  fe9efeeb  bl 0x108520e4
108aa4e8  000050e3  cmp r0, #0x0
108aa4ec  0600001a  bne 0x108aa50c
108aa4f0  0600a0e1  cpy r0, r6
108aa4f4  1c89feeb  bl 0x1084c96c
108aa4f8  0420a0e1  cpy r2, r4
108aa4fc  0500a0e1  cpy r0, r5
108aa500  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108aa504  0010a0e3  mov r1, #0x0
108aa508  0e89feea  b 0x1084c948
108aa50c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
