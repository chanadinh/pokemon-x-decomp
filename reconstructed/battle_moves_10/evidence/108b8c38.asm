108b8c38  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b8c3c  0150a0e1  cpy r5, r1
108b8c40  0240a0e1  cpy r4, r2
108b8c44  0200a0e3  mov r0, #0x2
108b8c48  33cbfeeb  bl 0x1086b91c
108b8c4c  040050e1  cmp r0, r4
108b8c50  0a00001a  bne 0x108b8c80
108b8c54  0410a0e1  cpy r1, r4
108b8c58  0500a0e1  cpy r0, r5
108b8c5c  0f6efeeb  bl 0x108544a0
108b8c60  0010a0e3  mov r1, #0x0
108b8c64  b04bfeeb  bl 0x1084bb2c
108b8c68  000050e3  cmp r0, #0x0
108b8c6c  0300001a  bne 0x108b8c80
108b8c70  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b8c74  1b10a0e3  mov r1, #0x1b
108b8c78  2300a0e3  mov r0, #0x23
108b8c7c  14b3feea  b 0x108658d4
108b8c80  7080bde8  ldmia sp!,{r4,r5,r6,pc}
