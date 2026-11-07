108c19bc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c19c0  0150a0e1  cpy r5, r1
108c19c4  0240a0e1  cpy r4, r2
108c19c8  0400a0e3  mov r0, #0x4
108c19cc  d2a7feeb  bl 0x1086b91c
108c19d0  040050e1  cmp r0, r4
108c19d4  1700001a  bne 0x108c1a38
108c19d8  4700a0e3  mov r0, #0x47
108c19dc  cea7feeb  bl 0x1086b91c
108c19e0  000050e3  cmp r0, #0x0
108c19e4  1300001a  bne 0x108c1a38
108c19e8  1600a0e3  mov r0, #0x16
108c19ec  caa7feeb  bl 0x1086b91c
108c19f0  100050e3  cmp r0, #0x10
108c19f4  0f00001a  bne 0x108c1a38
108c19f8  0420a0e1  cpy r2, r4
108c19fc  0d10a0e3  mov r1, #0xd
108c1a00  0500a0e1  cpy r0, r5
108c1a04  5ca1feeb  bl 0x10869f7c
108c1a08  0010a0e1  cpy r1, r0
108c1a0c  0c40c0e5  strb r4, [r0,#0xc]
108c1a10  000090e5  ldr r0, [r0,#0x0]
108c1a14  0120a0e3  mov r2, #0x1
108c1a18  1620c1e5  strb r2, [r1,#0x16]
108c1a1c  0420c1e5  strb r2, [r1,#0x4]
108c1a20  020580e3  orr r0, r0, #0x800000
108c1a24  1720c1e5  strb r2, [r1,#0x17]
108c1a28  000081e5  str r0, [r1,#0x0]
108c1a2c  0500a0e1  cpy r0, r5
108c1a30  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c1a34  4398feea  b 0x10867b48
108c1a38  7080bde8  ldmia sp!,{r4,r5,r6,pc}
