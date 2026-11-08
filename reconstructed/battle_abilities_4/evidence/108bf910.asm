108bf910  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bf914  0150a0e1  cpy r5, r1
108bf918  0240a0e1  cpy r4, r2
108bf91c  0400a0e3  mov r0, #0x4
108bf920  fdaffeeb  bl 0x1086b91c
108bf924  040050e1  cmp r0, r4
108bf928  0a00001a  bne 0x108bf958
108bf92c  0410a0e1  cpy r1, r4
108bf930  0500a0e1  cpy r0, r5
108bf934  d952feeb  bl 0x108544a0
108bf938  0610a0e3  mov r1, #0x6
108bf93c  515cfeeb  bl 0x10856a88
108bf940  000050e3  cmp r0, #0x0
108bf944  0300000a  beq 0x108bf958
108bf948  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bf94c  021ba0e3  mov r1, #0x800
108bf950  3600a0e3  mov r0, #0x36
108bf954  4a35feea  b 0x1084ce84
108bf958  7080bde8  ldmia sp!,{r4,r5,r6,pc}
