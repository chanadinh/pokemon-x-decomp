108b96ac  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b96b0  0150a0e1  cpy r5, r1
108b96b4  0240a0e1  cpy r4, r2
108b96b8  0300a0e3  mov r0, #0x3
108b96bc  96c8feeb  bl 0x1086b91c
108b96c0  040050e1  cmp r0, r4
108b96c4  0c00001a  bne 0x108b96fc
108b96c8  0400a0e3  mov r0, #0x4
108b96cc  92c8feeb  bl 0x1086b91c
108b96d0  ff1000e2  and r1, r0, #0xff
108b96d4  0500a0e1  cpy r0, r5
108b96d8  706bfeeb  bl 0x108544a0
108b96dc  0210a0e3  mov r1, #0x2
108b96e0  9ed8feeb  bl 0x1086f960
108b96e4  000050e3  cmp r0, #0x0
108b96e8  0300000a  beq 0x108b96fc
108b96ec  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b96f0  0110a0e3  mov r1, #0x1
108b96f4  4100a0e3  mov r0, #0x41
108b96f8  75b0feea  b 0x108658d4
108b96fc  7080bde8  ldmia sp!,{r4,r5,r6,pc}
