108aac94  70402de9  stmdb sp!,{r4,r5,r6,lr}
108aac98  0150a0e1  cpy r5, r1
108aac9c  0240a0e1  cpy r4, r2
108aaca0  0200a0e3  mov r0, #0x2
108aaca4  1c03ffeb  bl 0x1086b91c
108aaca8  040050e1  cmp r0, r4
108aacac  0a00001a  bne 0x108aacdc
108aacb0  0420a0e1  cpy r2, r4
108aacb4  2c10a0e3  mov r1, #0x2c
108aacb8  0500a0e1  cpy r0, r5
108aacbc  aefcfeeb  bl 0x10869f7c
108aacc0  0010a0e1  cpy r1, r0
108aacc4  6400a0e3  mov r0, #0x64
108aacc8  0440c1e5  strb r4, [r1,#0x4]
108aaccc  0500c1e5  strb r0, [r1,#0x5]
108aacd0  0500a0e1  cpy r0, r5
108aacd4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108aacd8  9af3feea  b 0x10867b48
108aacdc  7080bde8  ldmia sp!,{r4,r5,r6,pc}
