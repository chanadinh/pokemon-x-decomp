108bd070  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bd074  0150a0e1  cpy r5, r1
108bd078  0240a0e1  cpy r4, r2
108bd07c  0200a0e3  mov r0, #0x2
108bd080  25bafeeb  bl 0x1086b91c
108bd084  040050e1  cmp r0, r4
108bd088  1f00001a  bne 0x108bd10c
108bd08c  0300a0e3  mov r0, #0x3
108bd090  21bafeeb  bl 0x1086b91c
108bd094  ff1000e2  and r1, r0, #0xff
108bd098  0400a0e1  cpy r0, r4
108bd09c  413dfeeb  bl 0x1084c5a8
108bd0a0  000050e3  cmp r0, #0x0
108bd0a4  1800001a  bne 0x108bd10c
108bd0a8  2100a0e3  mov r0, #0x21
108bd0ac  1abafeeb  bl 0x1086b91c
108bd0b0  000050e3  cmp r0, #0x0
108bd0b4  140000aa  bge 0x108bd10c
108bd0b8  0420a0e1  cpy r2, r4
108bd0bc  0d10a0e3  mov r1, #0xd
108bd0c0  0500a0e1  cpy r0, r5
108bd0c4  acb3feeb  bl 0x10869f7c
108bd0c8  00c090e5  ldr r12, [r0,#0x0]
108bd0cc  1830d0e5  ldrb r3, [r0,#0x18]
108bd0d0  0010a0e1  cpy r1, r0
108bd0d4  0360a0e3  mov r6, #0x3
108bd0d8  0220a0e3  mov r2, #0x2
108bd0dc  0460c1e5  strb r6, [r1,#0x4]
108bd0e0  1720c1e5  strb r2, [r1,#0x17]
108bd0e4  02c58ce3  orr r12, r12, #0x800000
108bd0e8  022083e3  orr r2, r3, #0x2
108bd0ec  00c081e5  str r12, [r1,#0x0]
108bd0f0  0100a0e3  mov r0, #0x1
108bd0f4  1820c1e5  strb r2, [r1,#0x18]
108bd0f8  1600c1e5  strb r0, [r1,#0x16]
108bd0fc  0500a0e1  cpy r0, r5
108bd100  0c40c1e5  strb r4, [r1,#0xc]
108bd104  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bd108  8eaafeea  b 0x10867b48
108bd10c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
