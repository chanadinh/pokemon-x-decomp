108ab2cc  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108ab2d0  0160a0e1  cpy r6, r1
108ab2d4  0250a0e1  cpy r5, r2
108ab2d8  0300a0e3  mov r0, #0x3
108ab2dc  8e01ffeb  bl 0x1086b91c
108ab2e0  050050e1  cmp r0, r5
108ab2e4  1800001a  bne 0x108ab34c
108ab2e8  0510a0e1  cpy r1, r5
108ab2ec  0600a0e1  cpy r0, r6
108ab2f0  6aa4feeb  bl 0x108544a0
108ab2f4  0070a0e1  cpy r7, r0
108ab2f8  0520a0e1  cpy r2, r5
108ab2fc  0610a0e3  mov r1, #0x6
108ab300  0600a0e1  cpy r0, r6
108ab304  1cfbfeeb  bl 0x10869f7c
108ab308  0040a0e1  cpy r4, r0
108ab30c  0650c0e5  strb r5, [r0,#0x6]
108ab310  0a10a0e3  mov r1, #0xa
108ab314  0700a0e1  cpy r0, r7
108ab318  f587feeb  bl 0x1084d2f4
108ab31c  b400c4e1  strh r0, [r4,#0x4]
108ab320  28209fe5  ldr r2, [0x108ab350]
108ab324  0210a0e3  mov r1, #0x2
108ab328  0c0084e2  add r0, r4, #0xc
108ab32c  e887feeb  bl 0x1084d2d4
108ab330  0510a0e1  cpy r1, r5
108ab334  0c0084e2  add r0, r4, #0xc
108ab338  d787feeb  bl 0x1084d29c
108ab33c  0410a0e1  cpy r1, r4
108ab340  0600a0e1  cpy r0, r6
108ab344  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108ab348  fef1feea  b 0x10867b48
108ab34c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
