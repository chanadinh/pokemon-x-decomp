108a6b90  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a6b94  0150a0e1  cpy r5, r1
108a6b98  0240a0e1  cpy r4, r2
108a6b9c  0300a0e3  mov r0, #0x3
108a6ba0  5d13ffeb  bl 0x1086b91c
108a6ba4  040050e1  cmp r0, r4
108a6ba8  0c00001a  bne 0x108a6be0
108a6bac  0410a0e1  cpy r1, r4
108a6bb0  0500a0e1  cpy r0, r5
108a6bb4  39b6feeb  bl 0x108544a0
108a6bb8  bc00d0e1  ldrh r0, [r0,#0xc]
108a6bbc  710050e3  cmp r0, #0x71
108a6bc0  0600001a  bne 0x108a6be0
108a6bc4  2d00a0e3  mov r0, #0x2d
108a6bc8  5313ffeb  bl 0x1086b91c
108a6bcc  020080e2  add r0, r0, #0x2
108a6bd0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a6bd4  ff1000e2  and r1, r0, #0xff
108a6bd8  2d00a0e3  mov r0, #0x2d
108a6bdc  3cfbfeea  b 0x108658d4
108a6be0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
