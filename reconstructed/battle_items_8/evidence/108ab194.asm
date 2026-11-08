108ab194  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108ab198  0060a0e1  cpy r6, r0
108ab19c  0150a0e1  cpy r5, r1
108ab1a0  0240a0e1  cpy r4, r2
108ab1a4  0170a0e3  mov r7, #0x1
108ab1a8  0400a0e3  mov r0, #0x4
108ab1ac  da01ffeb  bl 0x1086b91c
108ab1b0  040050e1  cmp r0, r4
108ab1b4  1600001a  bne 0x108ab214
108ab1b8  4700a0e3  mov r0, #0x47
108ab1bc  d601ffeb  bl 0x1086b91c
108ab1c0  000050e3  cmp r0, #0x0
108ab1c4  1200001a  bne 0x108ab214
108ab1c8  1600a0e3  mov r0, #0x16
108ab1cc  d201ffeb  bl 0x1086b91c
108ab1d0  0e0050e3  cmp r0, #0xe
108ab1d4  0e00001a  bne 0x108ab214
108ab1d8  0410a0e1  cpy r1, r4
108ab1dc  0500a0e1  cpy r0, r5
108ab1e0  aea4feeb  bl 0x108544a0
108ab1e4  0120a0e3  mov r2, #0x1
108ab1e8  0710a0e1  cpy r1, r7
108ab1ec  3d1cffeb  bl 0x108722e8
108ab1f0  000050e3  cmp r0, #0x0
108ab1f4  0600000a  beq 0x108ab214
108ab1f8  0600a0e1  cpy r0, r6
108ab1fc  da85feeb  bl 0x1084c96c
108ab200  0420a0e1  cpy r2, r4
108ab204  0500a0e1  cpy r0, r5
108ab208  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108ab20c  0010a0e3  mov r1, #0x0
108ab210  cc85feea  b 0x1084c948
108ab214  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
