108bcd28  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bcd2c  0150a0e1  cpy r5, r1
108bcd30  0240a0e1  cpy r4, r2
108bcd34  0400a0e3  mov r0, #0x4
108bcd38  f7bafeeb  bl 0x1086b91c
108bcd3c  040050e1  cmp r0, r4
108bcd40  1c00001a  bne 0x108bcdb8
108bcd44  4700a0e3  mov r0, #0x47
108bcd48  f3bafeeb  bl 0x1086b91c
108bcd4c  000050e3  cmp r0, #0x0
108bcd50  1800001a  bne 0x108bcdb8
108bcd54  1600a0e3  mov r0, #0x16
108bcd58  efbafeeb  bl 0x1086b91c
108bcd5c  ff0000e2  and r0, r0, #0xff
108bcd60  100050e3  cmp r0, #0x10
108bcd64  0200000a  beq 0x108bcd74
108bcd68  060050e3  cmp r0, #0x6
108bcd6c  07005013  cmpne r0, #0x7
108bcd70  1000001a  bne 0x108bcdb8
108bcd74  0420a0e1  cpy r2, r4
108bcd78  0d10a0e3  mov r1, #0xd
108bcd7c  0500a0e1  cpy r0, r5
108bcd80  7db4feeb  bl 0x10869f7c
108bcd84  003090e5  ldr r3, [r0,#0x0]
108bcd88  0010a0e1  cpy r1, r0
108bcd8c  0120a0e3  mov r2, #0x1
108bcd90  0c40c1e5  strb r4, [r1,#0xc]
108bcd94  023583e3  orr r3, r3, #0x800000
108bcd98  1620c1e5  strb r2, [r1,#0x16]
108bcd9c  0500a0e3  mov r0, #0x5
108bcda0  003081e5  str r3, [r1,#0x0]
108bcda4  0400c1e5  strb r0, [r1,#0x4]
108bcda8  0500a0e1  cpy r0, r5
108bcdac  1720c1e5  strb r2, [r1,#0x17]
108bcdb0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bcdb4  63abfeea  b 0x10867b48
108bcdb8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
