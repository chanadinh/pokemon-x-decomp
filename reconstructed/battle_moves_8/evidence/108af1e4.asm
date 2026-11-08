108af1e4  70402de9  stmdb sp!,{r4,r5,r6,lr}
108af1e8  0150a0e1  cpy r5, r1
108af1ec  0240a0e1  cpy r4, r2
108af1f0  0300a0e3  mov r0, #0x3
108af1f4  c8f1feeb  bl 0x1086b91c
108af1f8  040050e1  cmp r0, r4
108af1fc  1700001a  bne 0x108af260
108af200  0400a0e3  mov r0, #0x4
108af204  c4f1feeb  bl 0x1086b91c
108af208  ff1000e2  and r1, r0, #0xff
108af20c  0500a0e1  cpy r0, r5
108af210  a294feeb  bl 0x108544a0
108af214  0060a0e1  cpy r6, r0
108af218  0410a0e1  cpy r1, r4
108af21c  0500a0e1  cpy r0, r5
108af220  9e94feeb  bl 0x108544a0
108af224  1210a0e3  mov r1, #0x12
108af228  7c14ffeb  bl 0x10874420
108af22c  ff4000e2  and r4, r0, #0xff
108af230  1210a0e3  mov r1, #0x12
108af234  0600a0e1  cpy r0, r6
108af238  7814ffeb  bl 0x10874420
108af23c  020054e3  cmp r4, #0x2
108af240  ff0000e2  and r0, r0, #0xff
108af244  02005013  cmpne r0, #0x2
108af248  00005411  cmpne r4, r0
108af24c  0300001a  bne 0x108af260
108af250  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108af254  0110a0e3  mov r1, #0x1
108af258  4100a0e3  mov r0, #0x41
108af25c  9cd9feea  b 0x108658d4
108af260  7080bde8  ldmia sp!,{r4,r5,r6,pc}
