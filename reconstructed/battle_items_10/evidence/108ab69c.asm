108ab69c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108ab6a0  0140a0e1  cpy r4, r1
108ab6a4  0060a0e1  cpy r6, r0
108ab6a8  0250a0e1  cpy r5, r2
108ab6ac  0210a0e1  cpy r1, r2
108ab6b0  0400a0e1  cpy r0, r4
108ab6b4  79a3feeb  bl 0x108544a0
108ab6b8  a082feeb  bl 0x1084c140
108ab6bc  000050e3  cmp r0, #0x0
108ab6c0  0600000a  beq 0x108ab6e0
108ab6c4  0600a0e1  cpy r0, r6
108ab6c8  a784feeb  bl 0x1084c96c
108ab6cc  0520a0e1  cpy r2, r5
108ab6d0  0400a0e1  cpy r0, r4
108ab6d4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108ab6d8  0010a0e3  mov r1, #0x0
108ab6dc  9984feea  b 0x1084c948
108ab6e0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
