108ba794  70402de9  stmdb sp!,{r4,r5,r6,lr}
108ba798  0160a0e1  cpy r6, r1
108ba79c  0250a0e1  cpy r5, r2
108ba7a0  0300a0e3  mov r0, #0x3
108ba7a4  5cc4feeb  bl 0x1086b91c
108ba7a8  050050e1  cmp r0, r5
108ba7ac  0f00001a  bne 0x108ba7f0
108ba7b0  0520a0e1  cpy r2, r5
108ba7b4  0310a0e3  mov r1, #0x3
108ba7b8  0600a0e1  cpy r0, r6
108ba7bc  eebdfeeb  bl 0x10869f7c
108ba7c0  0040a0e1  cpy r4, r0
108ba7c4  28209fe5  ldr r2, [0x108ba7f4]
108ba7c8  0210a0e3  mov r1, #0x2
108ba7cc  040080e2  add r0, r0, #0x4
108ba7d0  bf4afeeb  bl 0x1084d2d4
108ba7d4  0510a0e1  cpy r1, r5
108ba7d8  040084e2  add r0, r4, #0x4
108ba7dc  ae4afeeb  bl 0x1084d29c
108ba7e0  0410a0e1  cpy r1, r4
108ba7e4  0600a0e1  cpy r0, r6
108ba7e8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108ba7ec  d5b4feea  b 0x10867b48
108ba7f0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
