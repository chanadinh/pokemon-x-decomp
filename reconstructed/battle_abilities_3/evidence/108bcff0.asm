108bcff0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bcff4  0150a0e1  cpy r5, r1
108bcff8  0240a0e1  cpy r4, r2
108bcffc  0200a0e3  mov r0, #0x2
108bd000  45bafeeb  bl 0x1086b91c
108bd004  040050e1  cmp r0, r4
108bd008  1700001a  bne 0x108bd06c
108bd00c  0410a0e1  cpy r1, r4
108bd010  0500a0e1  cpy r0, r5
108bd014  215dfeeb  bl 0x108544a0
108bd018  0010a0e3  mov r1, #0x0
108bd01c  8a3efeeb  bl 0x1084ca4c
108bd020  000050e3  cmp r0, #0x0
108bd024  1000000a  beq 0x108bd06c
108bd028  0420a0e1  cpy r2, r4
108bd02c  0d10a0e3  mov r1, #0xd
108bd030  0500a0e1  cpy r0, r5
108bd034  d0b3feeb  bl 0x10869f7c
108bd038  003090e5  ldr r3, [r0,#0x0]
108bd03c  0010a0e1  cpy r1, r0
108bd040  0500a0e3  mov r0, #0x5
108bd044  0c40c1e5  strb r4, [r1,#0xc]
108bd048  0400c1e5  strb r0, [r1,#0x4]
108bd04c  023583e3  orr r3, r3, #0x800000
108bd050  0120a0e3  mov r2, #0x1
108bd054  003081e5  str r3, [r1,#0x0]
108bd058  1620c1e5  strb r2, [r1,#0x16]
108bd05c  0500a0e1  cpy r0, r5
108bd060  1720c1e5  strb r2, [r1,#0x17]
108bd064  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bd068  b6aafeea  b 0x10867b48
108bd06c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
