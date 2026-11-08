108a7e18  38402de9  stmdb sp!,{r3,r4,r5,lr}
108a7e1c  0040a0e1  cpy r4, r0
108a7e20  0250a0e1  cpy r5, r2
108a7e24  0200a0e3  mov r0, #0x2
108a7e28  bb0effeb  bl 0x1086b91c
108a7e2c  050050e1  cmp r0, r5
108a7e30  1300001a  bne 0x108a7e84
108a7e34  5600a0e3  mov r0, #0x56
108a7e38  b70effeb  bl 0x1086b91c
108a7e3c  ff0000e2  and r0, r0, #0xff
108a7e40  010050e3  cmp r0, #0x1
108a7e44  00005013  cmpne r0, #0x0
108a7e48  0d00001a  bne 0x108a7e84
108a7e4c  1f00a0e3  mov r0, #0x1f
108a7e50  b10effeb  bl 0x1086b91c
108a7e54  00008de5  str r0, [sp,#0x0]
108a7e58  0400a0e1  cpy r0, r4
108a7e5c  0350a0e3  mov r5, #0x3
108a7e60  b803d0e1  ldrh r0, [r0,#0x38]
108a7e64  0510a0e1  cpy r1, r5
108a7e68  4194feeb  bl 0x1084cf74
108a7e6c  ff1000e2  and r1, r0, #0xff
108a7e70  0d00a0e1  cpy r0, sp
108a7e74  743bffeb  bl 0x10876c4c
108a7e78  00109de5  ldr r1, [sp,#0x0]
108a7e7c  1f00a0e3  mov r0, #0x1f
108a7e80  93f6feeb  bl 0x108658d4
108a7e84  3880bde8  ldmia sp!,{r3,r4,r5,pc}
