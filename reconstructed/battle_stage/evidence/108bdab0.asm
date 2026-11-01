108bdab0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bdab4  0150a0e1  cpy r5, r1
108bdab8  0240a0e1  cpy r4, r2
108bdabc  0200a0e3  mov r0, #0x2
108bdac0  95b7feeb  bl 0x1086b91c
108bdac4  040050e1  cmp r0, r4
108bdac8  1e00001a  bne 0x108bdb48
108bdacc  0300a0e3  mov r0, #0x3
108bdad0  91b7feeb  bl 0x1086b91c
108bdad4  ff1000e2  and r1, r0, #0xff
108bdad8  0400a0e1  cpy r0, r4
108bdadc  b13afeeb  bl 0x1084c5a8
108bdae0  000050e3  cmp r0, #0x0
108bdae4  1700001a  bne 0x108bdb48
108bdae8  2100a0e3  mov r0, #0x21
108bdaec  8ab7feeb  bl 0x1086b91c
108bdaf0  000050e3  cmp r0, #0x0
108bdaf4  130000aa  bge 0x108bdb48
108bdaf8  0420a0e1  cpy r2, r4
108bdafc  0d10a0e3  mov r1, #0xd
108bdb00  0500a0e1  cpy r0, r5
108bdb04  1cb1feeb  bl 0x10869f7c
108bdb08  00c090e5  ldr r12, [r0,#0x0]
108bdb0c  1830d0e5  ldrb r3, [r0,#0x18]
108bdb10  0010a0e1  cpy r1, r0
108bdb14  0120a0e3  mov r2, #0x1
108bdb18  02c58ce3  orr r12, r12, #0x800000
108bdb1c  0420c1e5  strb r2, [r1,#0x4]
108bdb20  0200a0e3  mov r0, #0x2
108bdb24  00c081e5  str r12, [r1,#0x0]
108bdb28  1700c1e5  strb r0, [r1,#0x17]
108bdb2c  023083e3  orr r3, r3, #0x2
108bdb30  1830c1e5  strb r3, [r1,#0x18]
108bdb34  1620c1e5  strb r2, [r1,#0x16]
108bdb38  0500a0e1  cpy r0, r5
108bdb3c  0c40c1e5  strb r4, [r1,#0xc]
108bdb40  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bdb44  ffa7feea  b 0x10867b48
108bdb48  7080bde8  ldmia sp!,{r4,r5,r6,pc}
