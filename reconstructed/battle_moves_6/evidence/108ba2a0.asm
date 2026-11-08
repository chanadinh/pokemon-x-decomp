108ba2a0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108ba2a4  0160a0e1  cpy r6, r1
108ba2a8  0250a0e1  cpy r5, r2
108ba2ac  0300a0e3  mov r0, #0x3
108ba2b0  99c5feeb  bl 0x1086b91c
108ba2b4  050050e1  cmp r0, r5
108ba2b8  0f00001a  bne 0x108ba2fc
108ba2bc  0520a0e1  cpy r2, r5
108ba2c0  0310a0e3  mov r1, #0x3
108ba2c4  0600a0e1  cpy r0, r6
108ba2c8  2bbffeeb  bl 0x10869f7c
108ba2cc  0040a0e1  cpy r4, r0
108ba2d0  28209fe5  ldr r2, [0x108ba300]
108ba2d4  0210a0e3  mov r1, #0x2
108ba2d8  040080e2  add r0, r0, #0x4
108ba2dc  fc4bfeeb  bl 0x1084d2d4
108ba2e0  0510a0e1  cpy r1, r5
108ba2e4  040084e2  add r0, r4, #0x4
108ba2e8  eb4bfeeb  bl 0x1084d29c
108ba2ec  0410a0e1  cpy r1, r4
108ba2f0  0600a0e1  cpy r0, r6
108ba2f4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108ba2f8  12b6feea  b 0x10867b48
108ba2fc  7080bde8  ldmia sp!,{r4,r5,r6,pc}
