108be8f8  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108be8fc  0160a0e1  cpy r6, r1
108be900  0240a0e1  cpy r4, r2
108be904  0400a0e3  mov r0, #0x4
108be908  03b4feeb  bl 0x1086b91c
108be90c  040050e1  cmp r0, r4
108be910  2c00001a  bne 0x108be9c8
108be914  4700a0e3  mov r0, #0x47
108be918  ffb3feeb  bl 0x1086b91c
108be91c  000050e3  cmp r0, #0x0
108be920  2800001a  bne 0x108be9c8
108be924  1200a0e3  mov r0, #0x12
108be928  fbb3feeb  bl 0x1086b91c
108be92c  7000ffe6  uxth r0, r0
108be930  0010a0e3  mov r1, #0x0
108be934  d806fdeb  bl 0x1080049c
108be938  000050e3  cmp r0, #0x0
108be93c  2100000a  beq 0x108be9c8
108be940  0300a0e3  mov r0, #0x3
108be944  f4b3feeb  bl 0x1086b91c
108be948  ff5000e2  and r5, r0, #0xff
108be94c  0510a0e1  cpy r1, r5
108be950  0600a0e1  cpy r0, r6
108be954  d156feeb  bl 0x108544a0
108be958  0070a0e1  cpy r7, r0
108be95c  b883feeb  bl 0x1085f844
108be960  000050e3  cmp r0, #0x0
108be964  1700001a  bne 0x108be9c8
108be968  0420a0e1  cpy r2, r4
108be96c  0610a0e3  mov r1, #0x6
108be970  0600a0e1  cpy r0, r6
108be974  80adfeeb  bl 0x10869f7c
108be978  0040a0e1  cpy r4, r0
108be97c  000090e5  ldr r0, [r0,#0x0]
108be980  0650c4e5  strb r5, [r4,#0x6]
108be984  0810a0e3  mov r1, #0x8
108be988  020580e3  orr r0, r0, #0x800000
108be98c  000084e5  str r0, [r4,#0x0]
108be990  0700a0e1  cpy r0, r7
108be994  563afeeb  bl 0x1084d2f4
108be998  b400c4e1  strh r0, [r4,#0x4]
108be99c  28209fe5  ldr r2, [0x108be9cc]
108be9a0  0210a0e3  mov r1, #0x2
108be9a4  0c0084e2  add r0, r4, #0xc
108be9a8  493afeeb  bl 0x1084d2d4
108be9ac  0510a0e1  cpy r1, r5
108be9b0  0c0084e2  add r0, r4, #0xc
108be9b4  383afeeb  bl 0x1084d29c
108be9b8  0410a0e1  cpy r1, r4
108be9bc  0600a0e1  cpy r0, r6
108be9c0  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108be9c4  5fa4feea  b 0x10867b48
108be9c8  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
