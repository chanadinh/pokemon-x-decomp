108b9bdc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b9be0  0150a0e1  cpy r5, r1
108b9be4  0240a0e1  cpy r4, r2
108b9be8  0200a0e3  mov r0, #0x2
108b9bec  4ac7feeb  bl 0x1086b91c
108b9bf0  040050e1  cmp r0, r4
108b9bf4  0a00001a  bne 0x108b9c24
108b9bf8  0420a0e1  cpy r2, r4
108b9bfc  2410a0e3  mov r1, #0x24
108b9c00  0500a0e1  cpy r0, r5
108b9c04  dcc0feeb  bl 0x10869f7c
108b9c08  0120a0e3  mov r2, #0x1
108b9c0c  0420c0e5  strb r2, [r0,#0x4]
108b9c10  0010a0e1  cpy r1, r0
108b9c14  0520c0e5  strb r2, [r0,#0x5]
108b9c18  0500a0e1  cpy r0, r5
108b9c1c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b9c20  c8b7feea  b 0x10867b48
108b9c24  7080bde8  ldmia sp!,{r4,r5,r6,pc}
