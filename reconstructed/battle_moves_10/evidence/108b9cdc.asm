108b9cdc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b9ce0  0150a0e1  cpy r5, r1
108b9ce4  0240a0e1  cpy r4, r2
108b9ce8  0200a0e3  mov r0, #0x2
108b9cec  0ac7feeb  bl 0x1086b91c
108b9cf0  040050e1  cmp r0, r4
108b9cf4  0a00001a  bne 0x108b9d24
108b9cf8  0410a0e1  cpy r1, r4
108b9cfc  0500a0e1  cpy r0, r5
108b9d00  e669feeb  bl 0x108544a0
108b9d04  0010a0e3  mov r1, #0x0
108b9d08  8747feeb  bl 0x1084bb2c
108b9d0c  030050e3  cmp r0, #0x3
108b9d10  0300003a  bcc 0x108b9d24
108b9d14  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b9d18  1b10a0e3  mov r1, #0x1b
108b9d1c  2300a0e3  mov r0, #0x23
108b9d20  ebaefeea  b 0x108658d4
108b9d24  7080bde8  ldmia sp!,{r4,r5,r6,pc}
