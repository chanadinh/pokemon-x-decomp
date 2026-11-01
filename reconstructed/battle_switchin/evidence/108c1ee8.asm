108c1ee8  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108c1eec  0250a0e1  cpy r5, r2
108c1ef0  0170a0e1  cpy r7, r1
108c1ef4  0200a0e3  mov r0, #0x2
108c1ef8  87a6feeb  bl 0x1086b91c
108c1efc  050050e1  cmp r0, r5
108c1f00  2c00001a  bne 0x108c1fb8
108c1f04  0510a0e1  cpy r1, r5
108c1f08  0700a0e1  cpy r0, r7
108c1f0c  192bfeeb  bl 0x1084cb78
108c1f10  014c80e3  orr r4, r0, #0x100
108c1f14  0a10a0e3  mov r1, #0xa
108c1f18  0700a0e1  cpy r0, r7
108c1f1c  2b2afeeb  bl 0x1084c7d0
108c1f20  0060a0e1  cpy r6, r0
108c1f24  0020a0e1  cpy r2, r0
108c1f28  0410a0e1  cpy r1, r4
108c1f2c  0700a0e1  cpy r0, r7
108c1f30  3a27feeb  bl 0x1084bc20
108c1f34  0040b0e1  movs r4, r0
108c1f38  1e00000a  beq 0x108c1fb8
108c1f3c  0520a0e1  cpy r2, r5
108c1f40  0110a0e3  mov r1, #0x1
108c1f44  0700a0e1  cpy r0, r7
108c1f48  7e2afeeb  bl 0x1084c948
108c1f4c  0520a0e1  cpy r2, r5
108c1f50  0d10a0e3  mov r1, #0xd
108c1f54  0700a0e1  cpy r0, r7
108c1f58  07a0feeb  bl 0x10869f7c
108c1f5c  1830d0e5  ldrb r3, [r0,#0x18]
108c1f60  01c0a0e3  mov r12, #0x1
108c1f64  04c0c0e5  strb r12, [r0,#0x4]
108c1f68  ff20a0e3  mov r2, #0xff
108c1f6c  1640c0e5  strb r4, [r0,#0x16]
108c1f70  1720c0e5  strb r2, [r0,#0x17]
108c1f74  022083e3  orr r2, r3, #0x2
108c1f78  0010a0e1  cpy r1, r0
108c1f7c  1820c0e5  strb r2, [r0,#0x18]
108c1f80  0000a0e3  mov r0, #0x0
108c1f84  0120d6e4  ldrb r2, [r6],#0x1
108c1f88  003081e0  add r3, r1, r0
108c1f8c  014054e2  subs r4, r4, #0x1
108c1f90  010080e2  add r0, r0, #0x1
108c1f94  0c20c3e5  strb r2, [r3,#0xc]
108c1f98  f9ffff1a  bne 0x108c1f84
108c1f9c  0700a0e1  cpy r0, r7
108c1fa0  e896feeb  bl 0x10867b48
108c1fa4  0520a0e1  cpy r2, r5
108c1fa8  0700a0e1  cpy r0, r7
108c1fac  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108c1fb0  0210a0e3  mov r1, #0x2
108c1fb4  632afeea  b 0x1084c948
108c1fb8  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
