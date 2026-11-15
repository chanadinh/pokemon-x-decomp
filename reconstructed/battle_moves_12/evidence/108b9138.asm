108b9138  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b913c  0140a0e1  cpy r4, r1
108b9140  0250a0e1  cpy r5, r2
108b9144  0200a0e3  mov r0, #0x2
108b9148  f3c9feeb  bl 0x1086b91c
108b914c  050050e1  cmp r0, r5
108b9150  1800001a  bne 0x108b91b8
108b9154  0470a0e1  cpy r7, r4
108b9158  0510a0e1  cpy r1, r5
108b915c  0400a0e1  cpy r0, r4
108b9160  ce6cfeeb  bl 0x108544a0
108b9164  0210a0e3  mov r1, #0x2
108b9168  6150feeb  bl 0x1084d2f4
108b916c  0060a0e1  cpy r6, r0
108b9170  0520a0e1  cpy r2, r5
108b9174  0610a0e3  mov r1, #0x6
108b9178  0700a0e1  cpy r0, r7
108b917c  7ec3feeb  bl 0x10869f7c
108b9180  0650c0e5  strb r5, [r0,#0x6]
108b9184  0040a0e1  cpy r4, r0
108b9188  b460c0e1  strh r6, [r0,#0x4]
108b918c  e32fa0e3  mov r2, #0x38c
108b9190  0210a0e3  mov r1, #0x2
108b9194  0c0080e2  add r0, r0, #0xc
108b9198  4d50feeb  bl 0x1084d2d4
108b919c  0510a0e1  cpy r1, r5
108b91a0  0c0084e2  add r0, r4, #0xc
108b91a4  3c50feeb  bl 0x1084d29c
108b91a8  0410a0e1  cpy r1, r4
108b91ac  0700a0e1  cpy r0, r7
108b91b0  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108b91b4  63bafeea  b 0x10867b48
108b91b8  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
