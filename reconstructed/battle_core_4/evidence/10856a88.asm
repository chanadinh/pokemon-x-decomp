10856a88  70402de9  stmdb sp!,{r4,r5,r6,lr}
10856a8c  260051e3  cmp r1, #0x26
10856a90  0050a0e1  cpy r5, r0
10856a94  0140a0e1  cpy r4, r1
10856a98  0300003a  bcc 0x10856aac
10856a9c  0020a0e3  mov r2, #0x0
10856aa0  0210a0e1  cpy r1, r2
10856aa4  0200a0e1  cpy r0, r2
10856aa8  39a6feeb  bl 0x10800394
10856aac  040185e0  add r0, r5, r4, lsl #0x2
10856ab0  1c0090e5  ldr r0, [r0,#0x1c]
10856ab4  070010e2  ands r0, r0, #0x7
10856ab8  0100a013  movne r0, #0x1
10856abc  7080bde8  ldmia sp!,{r4,r5,r6,pc}
