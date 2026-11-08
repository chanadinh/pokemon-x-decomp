108bb560  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bb564  0150a0e1  cpy r5, r1
108bb568  0240a0e1  cpy r4, r2
108bb56c  0300a0e3  mov r0, #0x3
108bb570  e9c0feeb  bl 0x1086b91c
108bb574  040050e1  cmp r0, r4
108bb578  1900001a  bne 0x108bb5e4
108bb57c  0410a0e1  cpy r1, r4
108bb580  0500a0e1  cpy r0, r5
108bb584  c563feeb  bl 0x108544a0
108bb588  0040a0e1  cpy r4, r0
108bb58c  0400a0e3  mov r0, #0x4
108bb590  e1c0feeb  bl 0x1086b91c
108bb594  ff1000e2  and r1, r0, #0xff
108bb598  0500a0e1  cpy r0, r5
108bb59c  bf63feeb  bl 0x108544a0
108bb5a0  0050a0e1  cpy r5, r0
108bb5a4  1210a0e3  mov r1, #0x12
108bb5a8  0400a0e1  cpy r0, r4
108bb5ac  9be3feeb  bl 0x10874420
108bb5b0  ff4000e2  and r4, r0, #0xff
108bb5b4  1210a0e3  mov r1, #0x12
108bb5b8  0500a0e1  cpy r0, r5
108bb5bc  97e3feeb  bl 0x10874420
108bb5c0  020054e3  cmp r4, #0x2
108bb5c4  ff0000e2  and r0, r0, #0xff
108bb5c8  02005013  cmpne r0, #0x2
108bb5cc  00005411  cmpne r4, r0
108bb5d0  0300001a  bne 0x108bb5e4
108bb5d4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bb5d8  0110a0e3  mov r1, #0x1
108bb5dc  4100a0e3  mov r0, #0x41
108bb5e0  bba8feea  b 0x108658d4
108bb5e4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
