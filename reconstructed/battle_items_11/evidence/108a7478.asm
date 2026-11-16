108a7478  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a747c  0150a0e1  cpy r5, r1
108a7480  0240a0e1  cpy r4, r2
108a7484  0200a0e3  mov r0, #0x2
108a7488  2311ffeb  bl 0x1086b91c
108a748c  040050e1  cmp r0, r4
108a7490  1500001a  bne 0x108a74ec
108a7494  0410a0e1  cpy r1, r4
108a7498  0500a0e1  cpy r0, r5
108a749c  ffb3feeb  bl 0x108544a0
108a74a0  0120a0e3  mov r2, #0x1
108a74a4  0210a0e3  mov r1, #0x2
108a74a8  8e2bffeb  bl 0x108722e8
108a74ac  000050e3  cmp r0, #0x0
108a74b0  0d00000a  beq 0x108a74ec
108a74b4  0420a0e1  cpy r2, r4
108a74b8  0d10a0e3  mov r1, #0xd
108a74bc  0500a0e1  cpy r0, r5
108a74c0  ad0affeb  bl 0x10869f7c
108a74c4  0010a0e1  cpy r1, r0
108a74c8  0120a0e3  mov r2, #0x1
108a74cc  0200a0e3  mov r0, #0x2
108a74d0  1620c1e5  strb r2, [r1,#0x16]
108a74d4  0400c1e5  strb r0, [r1,#0x4]
108a74d8  1720c1e5  strb r2, [r1,#0x17]
108a74dc  0500a0e1  cpy r0, r5
108a74e0  0c40c1e5  strb r4, [r1,#0xc]
108a74e4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a74e8  9601ffea  b 0x10867b48
108a74ec  7080bde8  ldmia sp!,{r4,r5,r6,pc}
