108a8a30  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a8a34  0150a0e1  cpy r5, r1
108a8a38  0240a0e1  cpy r4, r2
108a8a3c  0200a0e3  mov r0, #0x2
108a8a40  b50bffeb  bl 0x1086b91c
108a8a44  040050e1  cmp r0, r4
108a8a48  1500001a  bne 0x108a8aa4
108a8a4c  0410a0e1  cpy r1, r4
108a8a50  0500a0e1  cpy r0, r5
108a8a54  91aefeeb  bl 0x108544a0
108a8a58  0120a0e3  mov r2, #0x1
108a8a5c  0410a0e3  mov r1, #0x4
108a8a60  2026ffeb  bl 0x108722e8
108a8a64  000050e3  cmp r0, #0x0
108a8a68  0d00000a  beq 0x108a8aa4
108a8a6c  0420a0e1  cpy r2, r4
108a8a70  0d10a0e3  mov r1, #0xd
108a8a74  0500a0e1  cpy r0, r5
108a8a78  3f05ffeb  bl 0x10869f7c
108a8a7c  0010a0e1  cpy r1, r0
108a8a80  0120a0e3  mov r2, #0x1
108a8a84  0400a0e3  mov r0, #0x4
108a8a88  1620c1e5  strb r2, [r1,#0x16]
108a8a8c  0400c1e5  strb r0, [r1,#0x4]
108a8a90  1720c1e5  strb r2, [r1,#0x17]
108a8a94  0500a0e1  cpy r0, r5
108a8a98  0c40c1e5  strb r4, [r1,#0xc]
108a8a9c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a8aa0  28fcfeea  b 0x10867b48
108a8aa4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
