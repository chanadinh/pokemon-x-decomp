108aab94  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108aab98  0060a0e1  cpy r6, r0
108aab9c  0150a0e1  cpy r5, r1
108aaba0  0240a0e1  cpy r4, r2
108aaba4  0200a0e3  mov r0, #0x2
108aaba8  5b03ffeb  bl 0x1086b91c
108aabac  040050e1  cmp r0, r4
108aabb0  1d00001a  bne 0x108aac2c
108aabb4  0410a0e1  cpy r1, r4
108aabb8  0500a0e1  cpy r0, r5
108aabbc  37a6feeb  bl 0x108544a0
108aabc0  0600a0e1  cpy r0, r6
108aabc4  0370a0e3  mov r7, #0x3
108aabc8  b803d0e1  ldrh r0, [r0,#0x38]
108aabcc  0710a0e1  cpy r1, r7
108aabd0  e788feeb  bl 0x1084cf74
108aabd4  ff7000e2  and r7, r0, #0xff
108aabd8  0200a0e3  mov r0, #0x2
108aabdc  4e03ffeb  bl 0x1086b91c
108aabe0  040050e1  cmp r0, r4
108aabe4  1000001a  bne 0x108aac2c
108aabe8  2f00a0e3  mov r0, #0x2f
108aabec  4a03ffeb  bl 0x1086b91c
108aabf0  010010e3  tst r0, #0x1
108aabf4  0c00000a  beq 0x108aac2c
108aabf8  0720a0e1  cpy r2, r7
108aabfc  0410a0e1  cpy r1, r4
108aac00  0500a0e1  cpy r0, r5
108aac04  c986feeb  bl 0x1084c730
108aac08  000050e3  cmp r0, #0x0
108aac0c  0600000a  beq 0x108aac2c
108aac10  0600a0e1  cpy r0, r6
108aac14  5487feeb  bl 0x1084c96c
108aac18  0420a0e1  cpy r2, r4
108aac1c  0500a0e1  cpy r0, r5
108aac20  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108aac24  0010a0e3  mov r1, #0x0
108aac28  4687feea  b 0x1084c948
108aac2c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
