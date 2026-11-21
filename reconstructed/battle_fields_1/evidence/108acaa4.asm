108acaa4  70402de9  stmdb sp!,{r4,r5,r6,lr}
108acaa8  0240a0e1  cpy r4, r2
108acaac  0400a0e3  mov r0, #0x4
108acab0  99fbfeeb  bl 0x1086b91c
108acab4  ff0000e2  and r0, r0, #0xff
108acab8  407dfeeb  bl 0x1084bfc0
108acabc  040050e1  cmp r0, r4
108acac0  1800001a  bne 0x108acb28
108acac4  1200a0e3  mov r0, #0x12
108acac8  93fbfeeb  bl 0x1086b91c
108acacc  7040ffe6  uxth r4, r0
108acad0  0400a0e1  cpy r0, r4
108acad4  564efdeb  bl 0x10800434
108acad8  000050e3  cmp r0, #0x0
108acadc  1100000a  beq 0x108acb28
108acae0  0110a0e3  mov r1, #0x1
108acae4  4100a0e3  mov r0, #0x41
108acae8  79e3feeb  bl 0x108658d4
108acaec  000050e3  cmp r0, #0x0
108acaf0  0c00000a  beq 0x108acb28
108acaf4  4000a0e3  mov r0, #0x40
108acaf8  87fbfeeb  bl 0x1086b91c
108acafc  0050a0e1  cpy r5, r0
108acb00  d520a0e3  mov r2, #0xd5
108acb04  0110a0e3  mov r1, #0x1
108acb08  f181feeb  bl 0x1084d2d4
108acb0c  0410a0e1  cpy r1, r4
108acb10  0500a0e1  cpy r0, r5
108acb14  e081feeb  bl 0x1084d29c
108acb18  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108acb1c  0110a0e3  mov r1, #0x1
108acb20  5300a0e3  mov r0, #0x53
108acb24  6ae3feea  b 0x108658d4
108acb28  7080bde8  ldmia sp!,{r4,r5,r6,pc}
