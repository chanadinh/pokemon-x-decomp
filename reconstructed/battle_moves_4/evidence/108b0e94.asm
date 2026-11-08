108b0e94  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b0e98  0150a0e1  cpy r5, r1
108b0e9c  0240a0e1  cpy r4, r2
108b0ea0  0300a0e3  mov r0, #0x3
108b0ea4  9ceafeeb  bl 0x1086b91c
108b0ea8  040050e1  cmp r0, r4
108b0eac  0c00001a  bne 0x108b0ee4
108b0eb0  0400a0e3  mov r0, #0x4
108b0eb4  98eafeeb  bl 0x1086b91c
108b0eb8  ff1000e2  and r1, r0, #0xff
108b0ebc  0500a0e1  cpy r0, r5
108b0ec0  768dfeeb  bl 0x108544a0
108b0ec4  0b10a0e3  mov r1, #0xb
108b0ec8  a4fafeeb  bl 0x1086f960
108b0ecc  000050e3  cmp r0, #0x0
108b0ed0  0300001a  bne 0x108b0ee4
108b0ed4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b0ed8  0010a0e3  mov r1, #0x0
108b0edc  2000a0e3  mov r0, #0x20
108b0ee0  7bd2feea  b 0x108658d4
108b0ee4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
