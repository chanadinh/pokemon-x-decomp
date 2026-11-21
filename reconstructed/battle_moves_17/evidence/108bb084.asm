108bb084  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bb088  0150a0e1  cpy r5, r1
108bb08c  0240a0e1  cpy r4, r2
108bb090  0300a0e3  mov r0, #0x3
108bb094  20c2feeb  bl 0x1086b91c
108bb098  040050e1  cmp r0, r4
108bb09c  0700001a  bne 0x108bb0c0
108bb0a0  0410a0e1  cpy r1, r4
108bb0a4  0500a0e1  cpy r0, r5
108bb0a8  b246feeb  bl 0x1084cb78
108bb0ac  bf44feeb  bl 0x1084c3b0
108bb0b0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bb0b4  0010a0e1  cpy r1, r0
108bb0b8  1f00a0e3  mov r0, #0x1f
108bb0bc  04aafeea  b 0x108658d4
108bb0c0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
