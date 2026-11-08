108bd8fc  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108bd900  0160a0e1  cpy r6, r1
108bd904  0250a0e1  cpy r5, r2
108bd908  3a00a0e3  mov r0, #0x3a
108bd90c  02b8feeb  bl 0x1086b91c
108bd910  030050e3  cmp r0, #0x3
108bd914  1b00001a  bne 0x108bd988
108bd918  0200a0e3  mov r0, #0x2
108bd91c  feb7feeb  bl 0x1086b91c
108bd920  050050e1  cmp r0, r5
108bd924  1700001a  bne 0x108bd988
108bd928  0510a0e1  cpy r1, r5
108bd92c  0600a0e1  cpy r0, r6
108bd930  da5afeeb  bl 0x108544a0
108bd934  0070a0e1  cpy r7, r0
108bd938  0520a0e1  cpy r2, r5
108bd93c  0410a0e3  mov r1, #0x4
108bd940  0600a0e1  cpy r0, r6
108bd944  8cb1feeb  bl 0x10869f7c
108bd948  0040a0e1  cpy r4, r0
108bd94c  000090e5  ldr r0, [r0,#0x0]
108bd950  0650c4e5  strb r5, [r4,#0x6]
108bd954  1010a0e3  mov r1, #0x10
108bd958  020580e3  orr r0, r0, #0x800000
108bd95c  000084e5  str r0, [r4,#0x0]
108bd960  0700a0e1  cpy r0, r7
108bd964  623efeeb  bl 0x1084d2f4
108bd968  b400c4e1  strh r0, [r4,#0x4]
108bd96c  0410a0e1  cpy r1, r4
108bd970  0600a0e1  cpy r0, r6
108bd974  73a8feeb  bl 0x10867b48
108bd978  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108bd97c  0110a0e3  mov r1, #0x1
108bd980  4200a0e3  mov r0, #0x42
108bd984  d29ffeea  b 0x108658d4
108bd988  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
