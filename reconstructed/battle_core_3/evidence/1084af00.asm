1084af00  10402de9  stmdb sp!,{r4,lr}
1084af04  020050e3  cmp r0, #0x2
1084af08  0040a0e1  cpy r4, r0
1084af0c  0300003a  bcc 0x1084af20
1084af10  0020a0e3  mov r2, #0x0
1084af14  0210a0e1  cpy r1, r2
1084af18  0200a0e1  cpy r0, r2
1084af1c  1cd5feeb  bl 0x10800394
1084af20  000054e3  cmp r4, #0x0
1084af24  0100a003  moveq r0, #0x1
1084af28  0000a013  movne r0, #0x0
1084af2c  1080bde8  ldmia sp!,{r4,pc}
