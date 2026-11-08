108b8cc4  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b8cc8  0150a0e1  cpy r5, r1
108b8ccc  0240a0e1  cpy r4, r2
108b8cd0  0200a0e3  mov r0, #0x2
108b8cd4  10cbfeeb  bl 0x1086b91c
108b8cd8  040050e1  cmp r0, r4
108b8cdc  1500001a  bne 0x108b8d38
108b8ce0  0410a0e1  cpy r1, r4
108b8ce4  0500a0e1  cpy r0, r5
108b8ce8  ec6dfeeb  bl 0x108544a0
108b8cec  0040a0e1  cpy r4, r0
108b8cf0  2300a0e3  mov r0, #0x23
108b8cf4  08cbfeeb  bl 0x1086b91c
108b8cf8  ff0000e2  and r0, r0, #0xff
108b8cfc  020050e3  cmp r0, #0x2
108b8d00  0800000a  beq 0x108b8d28
108b8d04  0210a0e3  mov r1, #0x2
108b8d08  0400a0e1  cpy r0, r4
108b8d0c  5d77feeb  bl 0x10856a88
108b8d10  000050e3  cmp r0, #0x0
108b8d14  0700001a  bne 0x108b8d38
108b8d18  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b8d1c  1b10a0e3  mov r1, #0x1b
108b8d20  2300a0e3  mov r0, #0x23
108b8d24  eab2feea  b 0x108658d4
108b8d28  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b8d2c  0010a0e3  mov r1, #0x0
108b8d30  2300a0e3  mov r0, #0x23
108b8d34  e6b2feea  b 0x108658d4
108b8d38  7080bde8  ldmia sp!,{r4,r5,r6,pc}
