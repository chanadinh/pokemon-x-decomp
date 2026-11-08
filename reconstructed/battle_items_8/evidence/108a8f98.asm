108a8f98  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108a8f9c  0060a0e1  cpy r6, r0
108a8fa0  0350a0e1  cpy r5, r3
108a8fa4  0170a0e1  cpy r7, r1
108a8fa8  0240a0e1  cpy r4, r2
108a8fac  0400a0e3  mov r0, #0x4
108a8fb0  590affeb  bl 0x1086b91c
108a8fb4  040050e1  cmp r0, r4
108a8fb8  1e00001a  bne 0x108a9038
108a8fbc  4700a0e3  mov r0, #0x47
108a8fc0  550affeb  bl 0x1086b91c
108a8fc4  000050e3  cmp r0, #0x0
108a8fc8  1a00001a  bne 0x108a9038
108a8fcc  1200a0e3  mov r0, #0x12
108a8fd0  510affeb  bl 0x1086b91c
108a8fd4  7000ffe6  uxth r0, r0
108a8fd8  0a8efeeb  bl 0x1084c808
108a8fdc  000050e3  cmp r0, #0x0
108a8fe0  1400001a  bne 0x108a9038
108a8fe4  1b00a0e3  mov r0, #0x1b
108a8fe8  4b0affeb  bl 0x1086b91c
108a8fec  010050e3  cmp r0, #0x1
108a8ff0  1000001a  bne 0x108a9038
108a8ff4  0600a0e1  cpy r0, r6
108a8ff8  5b8efeeb  bl 0x1084c96c
108a8ffc  0300a0e3  mov r0, #0x3
108a9000  450affeb  bl 0x1086b91c
108a9004  010080e2  add r0, r0, #0x1
108a9008  000085e5  str r0, [r5,#0x0]
108a900c  0420a0e1  cpy r2, r4
108a9010  0010a0e3  mov r1, #0x0
108a9014  0700a0e1  cpy r0, r7
108a9018  d703ffeb  bl 0x10869f7c
108a901c  0010a0e1  cpy r1, r0
108a9020  040090e5  ldr r0, [r0,#0x4]
108a9024  020080e3  orr r0, r0, #0x2
108a9028  040081e5  str r0, [r1,#0x4]
108a902c  0700a0e1  cpy r0, r7
108a9030  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108a9034  c3fafeea  b 0x10867b48
108a9038  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
