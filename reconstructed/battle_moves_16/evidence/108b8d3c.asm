108b8d3c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b8d40  0150a0e1  cpy r5, r1
108b8d44  0240a0e1  cpy r4, r2
108b8d48  0300a0e3  mov r0, #0x3
108b8d4c  f2cafeeb  bl 0x1086b91c
108b8d50  040050e1  cmp r0, r4
108b8d54  0700001a  bne 0x108b8d78
108b8d58  0500a0e1  cpy r0, r5
108b8d5c  3d4cfeeb  bl 0x1084be58
108b8d60  010050e3  cmp r0, #0x1
108b8d64  0300001a  bne 0x108b8d78
108b8d68  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b8d6c  3210a0e3  mov r1, #0x32
108b8d70  2c00a0e3  mov r0, #0x2c
108b8d74  d6b2feea  b 0x108658d4
108b8d78  7080bde8  ldmia sp!,{r4,r5,r6,pc}
