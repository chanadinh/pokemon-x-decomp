108ac568  10402de9  stmdb sp!,{r4,lr}
108ac56c  0240a0e1  cpy r4, r2
108ac570  0400a0e3  mov r0, #0x4
108ac574  e8fcfeeb  bl 0x1086b91c
108ac578  ff0000e2  and r0, r0, #0xff
108ac57c  8f7efeeb  bl 0x1084bfc0
108ac580  040050e1  cmp r0, r4
108ac584  0300001a  bne 0x108ac598
108ac588  1040bde8  ldmia sp!,{r4,lr}
108ac58c  0110a0e3  mov r1, #0x1
108ac590  4200a0e3  mov r0, #0x42
108ac594  cee4feea  b 0x108658d4
108ac598  1080bde8  ldmia sp!,{r4,pc}
