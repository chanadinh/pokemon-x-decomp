108a81b8  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108a81bc  0160a0e1  cpy r6, r1
108a81c0  0070a0e1  cpy r7, r0
108a81c4  0250a0e1  cpy r5, r2
108a81c8  0200a0e3  mov r0, #0x2
108a81cc  d20dffeb  bl 0x1086b91c
108a81d0  050050e1  cmp r0, r5
108a81d4  1c00001a  bne 0x108a824c
108a81d8  0520a0e1  cpy r2, r5
108a81dc  1110a0e3  mov r1, #0x11
108a81e0  0600a0e1  cpy r0, r6
108a81e4  6407ffeb  bl 0x10869f7c
108a81e8  0010a0e1  cpy r1, r0
108a81ec  0450c0e5  strb r5, [r0,#0x4]
108a81f0  0600a0e1  cpy r0, r6
108a81f4  53fefeeb  bl 0x10867b48
108a81f8  0520a0e1  cpy r2, r5
108a81fc  0310a0e3  mov r1, #0x3
108a8200  0600a0e1  cpy r0, r6
108a8204  5c07ffeb  bl 0x10869f7c
108a8208  0040a0e1  cpy r4, r0
108a820c  fe2fa0e3  mov r2, #0x3f8
108a8210  0210a0e3  mov r1, #0x2
108a8214  040080e2  add r0, r0, #0x4
108a8218  2d94feeb  bl 0x1084d2d4
108a821c  0510a0e1  cpy r1, r5
108a8220  040084e2  add r0, r4, #0x4
108a8224  1c94feeb  bl 0x1084d29c
108a8228  0700a0e1  cpy r0, r7
108a822c  b803d0e1  ldrh r0, [r0,#0x38]
108a8230  0010a0e1  cpy r1, r0
108a8234  040084e2  add r0, r4, #0x4
108a8238  1794feeb  bl 0x1084d29c
108a823c  0410a0e1  cpy r1, r4
108a8240  0600a0e1  cpy r0, r6
108a8244  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108a8248  3efefeea  b 0x10867b48
108a824c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
