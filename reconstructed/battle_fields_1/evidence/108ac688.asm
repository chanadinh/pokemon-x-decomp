108ac688  70402de9  stmdb sp!,{r4,r5,r6,lr}
108ac68c  0150a0e1  cpy r5, r1
108ac690  0240a0e1  cpy r4, r2
108ac694  0400a0e3  mov r0, #0x4
108ac698  9ffcfeeb  bl 0x1086b91c
108ac69c  ff0000e2  and r0, r0, #0xff
108ac6a0  467efeeb  bl 0x1084bfc0
108ac6a4  040050e1  cmp r0, r4
108ac6a8  1300001a  bne 0x108ac6fc
108ac6ac  1b00a0e3  mov r0, #0x1b
108ac6b0  99fcfeeb  bl 0x1086b91c
108ac6b4  010050e3  cmp r0, #0x1
108ac6b8  0f00001a  bne 0x108ac6fc
108ac6bc  4600a0e3  mov r0, #0x46
108ac6c0  95fcfeeb  bl 0x1086b91c
108ac6c4  000050e3  cmp r0, #0x0
108ac6c8  0b00001a  bne 0x108ac6fc
108ac6cc  0500a0e1  cpy r0, r5
108ac6d0  0f7efeeb  bl 0x1084bf14
108ac6d4  000050e3  cmp r0, #0x0
108ac6d8  0500000a  beq 0x108ac6f4
108ac6dc  030050e3  cmp r0, #0x3
108ac6e0  18109f15  ldrne r1, [0x108ac700]
108ac6e4  0200000a  beq 0x108ac6f4
108ac6e8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108ac6ec  3600a0e3  mov r0, #0x36
108ac6f0  e381feea  b 0x1084ce84
108ac6f4  021ba0e3  mov r1, #0x800
108ac6f8  faffffea  b 0x108ac6e8
108ac6fc  7080bde8  ldmia sp!,{r4,r5,r6,pc}
