108b4d08  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b4d0c  0160a0e1  cpy r6, r1
108b4d10  0240a0e1  cpy r4, r2
108b4d14  0300a0e3  mov r0, #0x3
108b4d18  ffdafeeb  bl 0x1086b91c
108b4d1c  040050e1  cmp r0, r4
108b4d20  2300001a  bne 0x108b4db4
108b4d24  0410a0e1  cpy r1, r4
108b4d28  0600a0e1  cpy r0, r6
108b4d2c  db7dfeeb  bl 0x108544a0
108b4d30  0400a0e3  mov r0, #0x4
108b4d34  f8dafeeb  bl 0x1086b91c
108b4d38  ff1000e2  and r1, r0, #0xff
108b4d3c  0600a0e1  cpy r0, r6
108b4d40  d67dfeeb  bl 0x108544a0
108b4d44  0050a0e1  cpy r5, r0
108b4d48  0410a0e1  cpy r1, r4
108b4d4c  0600a0e1  cpy r0, r6
108b4d50  265bfeeb  bl 0x1084b9f0
108b4d54  0040a0e1  cpy r4, r0
108b4d58  0500a0e1  cpy r0, r5
108b4d5c  1900d0e5  ldrb r0, [r0,#0x19]
108b4d60  0010a0e1  cpy r1, r0
108b4d64  0600a0e1  cpy r0, r6
108b4d68  205bfeeb  bl 0x1084b9f0
108b4d6c  0010a0e1  cpy r1, r0
108b4d70  0400a0e1  cpy r0, r4
108b4d74  2e2ffdeb  bl 0x10800a34
108b4d78  050050e3  cmp r0, #0x5
108b4d7c  7810a0a3  movge r1, #0x78
108b4d80  080000aa  bge 0x108b4da8
108b4d84  040050e3  cmp r0, #0x4
108b4d88  6410a003  moveq r1, #0x64
108b4d8c  0500000a  beq 0x108b4da8
108b4d90  030050e3  cmp r0, #0x3
108b4d94  5010a003  moveq r1, #0x50
108b4d98  0200000a  beq 0x108b4da8
108b4d9c  020050e3  cmp r0, #0x2
108b4da0  3c10a003  moveq r1, #0x3c
108b4da4  2810a013  movne r1, #0x28
108b4da8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b4dac  3100a0e3  mov r0, #0x31
108b4db0  c7c2feea  b 0x108658d4
108b4db4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
