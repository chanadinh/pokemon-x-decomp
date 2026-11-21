108ae800  70402de9  stmdb sp!,{r4,r5,r6,lr}
108ae804  0150a0e1  cpy r5, r1
108ae808  0240a0e1  cpy r4, r2
108ae80c  0300a0e3  mov r0, #0x3
108ae810  41f4feeb  bl 0x1086b91c
108ae814  040050e1  cmp r0, r4
108ae818  0c00001a  bne 0x108ae850
108ae81c  0420a0e1  cpy r2, r4
108ae820  0310a0e3  mov r1, #0x3
108ae824  0500a0e1  cpy r0, r5
108ae828  d3edfeeb  bl 0x10869f7c
108ae82c  0040a0e1  cpy r4, r0
108ae830  6a20a0e3  mov r2, #0x6a
108ae834  0110a0e3  mov r1, #0x1
108ae838  040080e2  add r0, r0, #0x4
108ae83c  a47afeeb  bl 0x1084d2d4
108ae840  0410a0e1  cpy r1, r4
108ae844  0500a0e1  cpy r0, r5
108ae848  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108ae84c  bde4feea  b 0x10867b48
108ae850  7080bde8  ldmia sp!,{r4,r5,r6,pc}
