108bc458  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108bc45c  1050a0e3  mov r5, #0x10
108bc460  0170a0e1  cpy r7, r1
108bc464  90809fe5  ldr r8, [0x108bc4fc]
108bc468  0400a0e3  mov r0, #0x4
108bc46c  2abdfeeb  bl 0x1086b91c
108bc470  ff6000e2  and r6, r0, #0xff
108bc474  0610a0e1  cpy r1, r6
108bc478  0700a0e1  cpy r0, r7
108bc47c  0760feeb  bl 0x108544a0
108bc480  0510a0e1  cpy r1, r5
108bc484  7041feeb  bl 0x1084ca4c
108bc488  000050e3  cmp r0, #0x0
108bc48c  1900000a  beq 0x108bc4f8
108bc490  0620a0e1  cpy r2, r6
108bc494  1810a0e3  mov r1, #0x18
108bc498  0700a0e1  cpy r0, r7
108bc49c  b6b6feeb  bl 0x10869f7c
108bc4a0  0450c0e5  strb r5, [r0,#0x4]
108bc4a4  0040a0e1  cpy r4, r0
108bc4a8  0010a0e1  cpy r1, r0
108bc4ac  0560c0e5  strb r6, [r0,#0x5]
108bc4b0  0700a0e1  cpy r0, r7
108bc4b4  a3adfeeb  bl 0x10867b48
108bc4b8  0620a0e1  cpy r2, r6
108bc4bc  0310a0e3  mov r1, #0x3
108bc4c0  0700a0e1  cpy r0, r7
108bc4c4  acb6feeb  bl 0x10869f7c
108bc4c8  0050a0e1  cpy r5, r0
108bc4cc  0820a0e1  cpy r2, r8
108bc4d0  0210a0e3  mov r1, #0x2
108bc4d4  040080e2  add r0, r0, #0x4
108bc4d8  7d43feeb  bl 0x1084d2d4
108bc4dc  0610a0e1  cpy r1, r6
108bc4e0  040085e2  add r0, r5, #0x4
108bc4e4  6c43feeb  bl 0x1084d29c
108bc4e8  0410a0e1  cpy r1, r4
108bc4ec  0700a0e1  cpy r0, r7
108bc4f0  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108bc4f4  93adfeea  b 0x10867b48
108bc4f8  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
