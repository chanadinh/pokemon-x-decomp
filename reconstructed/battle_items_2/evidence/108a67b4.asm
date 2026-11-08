108a67b4  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a67b8  0250a0e3  mov r5, #0x2
108a67bc  0160a0e1  cpy r6, r1
108a67c0  0240a0e1  cpy r4, r2
108a67c4  0500a0e1  cpy r0, r5
108a67c8  5314ffeb  bl 0x1086b91c
108a67cc  040050e1  cmp r0, r4
108a67d0  0c00001a  bne 0x108a6808
108a67d4  0420a0e1  cpy r2, r4
108a67d8  0d10a0e3  mov r1, #0xd
108a67dc  0600a0e1  cpy r0, r6
108a67e0  e50dffeb  bl 0x10869f7c
108a67e4  0120a0e3  mov r2, #0x1
108a67e8  0450c0e5  strb r5, [r0,#0x4]
108a67ec  1720c0e5  strb r2, [r0,#0x17]
108a67f0  0c40c0e5  strb r4, [r0,#0xc]
108a67f4  0010a0e1  cpy r1, r0
108a67f8  1620c0e5  strb r2, [r0,#0x16]
108a67fc  0600a0e1  cpy r0, r6
108a6800  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a6804  cf04ffea  b 0x10867b48
108a6808  7080bde8  ldmia sp!,{r4,r5,r6,pc}
