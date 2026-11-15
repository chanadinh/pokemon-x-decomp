108a98a0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a98a4  0060a0e1  cpy r6, r0
108a98a8  0150a0e1  cpy r5, r1
108a98ac  0240a0e1  cpy r4, r2
108a98b0  0200a0e3  mov r0, #0x2
108a98b4  1808ffeb  bl 0x1086b91c
108a98b8  040050e1  cmp r0, r4
108a98bc  0c00001a  bne 0x108a98f4
108a98c0  0410a0e1  cpy r1, r4
108a98c4  0500a0e1  cpy r0, r5
108a98c8  f4aafeeb  bl 0x108544a0
108a98cc  dc8bfeeb  bl 0x1084c844
108a98d0  000050e3  cmp r0, #0x0
108a98d4  0600000a  beq 0x108a98f4
108a98d8  0600a0e1  cpy r0, r6
108a98dc  228cfeeb  bl 0x1084c96c
108a98e0  0420a0e1  cpy r2, r4
108a98e4  0500a0e1  cpy r0, r5
108a98e8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a98ec  0010a0e3  mov r1, #0x0
108a98f0  148cfeea  b 0x1084c948
108a98f4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
