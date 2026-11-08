108a629c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a62a0  0150a0e1  cpy r5, r1
108a62a4  0240a0e1  cpy r4, r2
108a62a8  0300a0e3  mov r0, #0x3
108a62ac  9a15ffeb  bl 0x1086b91c
108a62b0  040050e1  cmp r0, r4
108a62b4  0c00001a  bne 0x108a62ec
108a62b8  0410a0e1  cpy r1, r4
108a62bc  0500a0e1  cpy r0, r5
108a62c0  76b8feeb  bl 0x108544a0
108a62c4  bc00d0e1  ldrh r0, [r0,#0xc]
108a62c8  530050e3  cmp r0, #0x53
108a62cc  0600001a  bne 0x108a62ec
108a62d0  2d00a0e3  mov r0, #0x2d
108a62d4  9015ffeb  bl 0x1086b91c
108a62d8  020080e2  add r0, r0, #0x2
108a62dc  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a62e0  ff1000e2  and r1, r0, #0xff
108a62e4  2d00a0e3  mov r0, #0x2d
108a62e8  79fdfeea  b 0x108658d4
108a62ec  7080bde8  ldmia sp!,{r4,r5,r6,pc}
