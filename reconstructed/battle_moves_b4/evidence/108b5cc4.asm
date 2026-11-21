108b5cc4  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b5cc8  0150a0e1  cpy r5, r1
108b5ccc  0240a0e1  cpy r4, r2
108b5cd0  0300a0e3  mov r0, #0x3
108b5cd4  10d7feeb  bl 0x1086b91c
108b5cd8  040050e1  cmp r0, r4
108b5cdc  4c00001a  bne 0x108b5e14
108b5ce0  0420a0e1  cpy r2, r4
108b5ce4  0d10a0e3  mov r1, #0xd
108b5ce8  0500a0e1  cpy r0, r5
108b5cec  a2d0feeb  bl 0x10869f7c
108b5cf0  0010a0e1  cpy r1, r0
108b5cf4  1800d0e5  ldrb r0, [r0,#0x18]
108b5cf8  0270a0e3  mov r7, #0x2
108b5cfc  0080e0e3  mvn r8, #0x0
108b5d00  0470c1e5  strb r7, [r1,#0x4]
108b5d04  0160a0e3  mov r6, #0x1
108b5d08  1780c1e5  strb r8, [r1,#0x17]
108b5d0c  060080e3  orr r0, r0, #0x6
108b5d10  1660c1e5  strb r6, [r1,#0x16]
108b5d14  1800c1e5  strb r0, [r1,#0x18]
108b5d18  0500a0e1  cpy r0, r5
108b5d1c  0c40c1e5  strb r4, [r1,#0xc]
108b5d20  88c7feeb  bl 0x10867b48
108b5d24  0420a0e1  cpy r2, r4
108b5d28  0d10a0e3  mov r1, #0xd
108b5d2c  0500a0e1  cpy r0, r5
108b5d30  91d0feeb  bl 0x10869f7c
108b5d34  1810d0e5  ldrb r1, [r0,#0x18]
108b5d38  0420a0e3  mov r2, #0x4
108b5d3c  0420c0e5  strb r2, [r0,#0x4]
108b5d40  1780c0e5  strb r8, [r0,#0x17]
108b5d44  1660c0e5  strb r6, [r0,#0x16]
108b5d48  061081e3  orr r1, r1, #0x6
108b5d4c  0c40c0e5  strb r4, [r0,#0xc]
108b5d50  1810c0e5  strb r1, [r0,#0x18]
108b5d54  0010a0e1  cpy r1, r0
108b5d58  0500a0e1  cpy r0, r5
108b5d5c  79c7feeb  bl 0x10867b48
108b5d60  0420a0e1  cpy r2, r4
108b5d64  0d10a0e3  mov r1, #0xd
108b5d68  0500a0e1  cpy r0, r5
108b5d6c  82d0feeb  bl 0x10869f7c
108b5d70  0010a0e1  cpy r1, r0
108b5d74  1800d0e5  ldrb r0, [r0,#0x18]
108b5d78  0460c1e5  strb r6, [r1,#0x4]
108b5d7c  1770c1e5  strb r7, [r1,#0x17]
108b5d80  1660c1e5  strb r6, [r1,#0x16]
108b5d84  060080e3  orr r0, r0, #0x6
108b5d88  0c40c1e5  strb r4, [r1,#0xc]
108b5d8c  1800c1e5  strb r0, [r1,#0x18]
108b5d90  0500a0e1  cpy r0, r5
108b5d94  6bc7feeb  bl 0x10867b48
108b5d98  0420a0e1  cpy r2, r4
108b5d9c  0d10a0e3  mov r1, #0xd
108b5da0  0500a0e1  cpy r0, r5
108b5da4  74d0feeb  bl 0x10869f7c
108b5da8  0010a0e1  cpy r1, r0
108b5dac  1800d0e5  ldrb r0, [r0,#0x18]
108b5db0  0320a0e3  mov r2, #0x3
108b5db4  0420c1e5  strb r2, [r1,#0x4]
108b5db8  1770c1e5  strb r7, [r1,#0x17]
108b5dbc  1660c1e5  strb r6, [r1,#0x16]
108b5dc0  060080e3  orr r0, r0, #0x6
108b5dc4  0c40c1e5  strb r4, [r1,#0xc]
108b5dc8  1800c1e5  strb r0, [r1,#0x18]
108b5dcc  0500a0e1  cpy r0, r5
108b5dd0  5cc7feeb  bl 0x10867b48
108b5dd4  0420a0e1  cpy r2, r4
108b5dd8  0d10a0e3  mov r1, #0xd
108b5ddc  0500a0e1  cpy r0, r5
108b5de0  65d0feeb  bl 0x10869f7c
108b5de4  0010a0e1  cpy r1, r0
108b5de8  1800d0e5  ldrb r0, [r0,#0x18]
108b5dec  0520a0e3  mov r2, #0x5
108b5df0  1770c1e5  strb r7, [r1,#0x17]
108b5df4  0420c1e5  strb r2, [r1,#0x4]
108b5df8  1660c1e5  strb r6, [r1,#0x16]
108b5dfc  060080e3  orr r0, r0, #0x6
108b5e00  0c40c1e5  strb r4, [r1,#0xc]
108b5e04  1800c1e5  strb r0, [r1,#0x18]
108b5e08  0500a0e1  cpy r0, r5
108b5e0c  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108b5e10  4cc7feea  b 0x10867b48
108b5e14  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
