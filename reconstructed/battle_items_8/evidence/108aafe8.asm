108aafe8  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108aafec  0160a0e1  cpy r6, r1
108aaff0  0070a0e1  cpy r7, r0
108aaff4  0240a0e1  cpy r4, r2
108aaff8  0350a0e1  cpy r5, r3
108aaffc  0400a0e3  mov r0, #0x4
108ab000  4502ffeb  bl 0x1086b91c
108ab004  040050e1  cmp r0, r4
108ab008  1800001a  bne 0x108ab070
108ab00c  4700a0e3  mov r0, #0x47
108ab010  4102ffeb  bl 0x1086b91c
108ab014  000050e3  cmp r0, #0x0
108ab018  1400001a  bne 0x108ab070
108ab01c  3900a0e3  mov r0, #0x39
108ab020  3d02ffeb  bl 0x1086b91c
108ab024  070050e3  cmp r0, #0x7
108ab028  100000da  ble 0x108ab070
108ab02c  0700a0e1  cpy r0, r7
108ab030  4d86feeb  bl 0x1084c96c
108ab034  0300a0e3  mov r0, #0x3
108ab038  3702ffeb  bl 0x1086b91c
108ab03c  010080e2  add r0, r0, #0x1
108ab040  000085e5  str r0, [r5,#0x0]
108ab044  0420a0e1  cpy r2, r4
108ab048  0010a0e3  mov r1, #0x0
108ab04c  0600a0e1  cpy r0, r6
108ab050  c9fbfeeb  bl 0x10869f7c
108ab054  0010a0e1  cpy r1, r0
108ab058  040090e5  ldr r0, [r0,#0x4]
108ab05c  020080e3  orr r0, r0, #0x2
108ab060  040081e5  str r0, [r1,#0x4]
108ab064  0600a0e1  cpy r0, r6
108ab068  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108ab06c  b5f2feea  b 0x10867b48
108ab070  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
