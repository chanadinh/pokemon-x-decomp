108ba9cc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108ba9d0  0160a0e1  cpy r6, r1
108ba9d4  0250a0e1  cpy r5, r2
108ba9d8  0300a0e3  mov r0, #0x3
108ba9dc  cec3feeb  bl 0x1086b91c
108ba9e0  050050e1  cmp r0, r5
108ba9e4  0f00001a  bne 0x108baa28
108ba9e8  0520a0e1  cpy r2, r5
108ba9ec  0310a0e3  mov r1, #0x3
108ba9f0  0600a0e1  cpy r0, r6
108ba9f4  60bdfeeb  bl 0x10869f7c
108ba9f8  0040a0e1  cpy r4, r0
108ba9fc  28209fe5  ldr r2, [0x108baa2c]
108baa00  0210a0e3  mov r1, #0x2
108baa04  040080e2  add r0, r0, #0x4
108baa08  314afeeb  bl 0x1084d2d4
108baa0c  0510a0e1  cpy r1, r5
108baa10  040084e2  add r0, r4, #0x4
108baa14  204afeeb  bl 0x1084d29c
108baa18  0410a0e1  cpy r1, r4
108baa1c  0600a0e1  cpy r0, r6
108baa20  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108baa24  47b4feea  b 0x10867b48
108baa28  7080bde8  ldmia sp!,{r4,r5,r6,pc}
