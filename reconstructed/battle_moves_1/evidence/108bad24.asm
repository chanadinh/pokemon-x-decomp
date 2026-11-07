108bad24  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bad28  0150a0e1  cpy r5, r1
108bad2c  0240a0e1  cpy r4, r2
108bad30  0300a0e3  mov r0, #0x3
108bad34  f8c2feeb  bl 0x1086b91c
108bad38  040050e1  cmp r0, r4
108bad3c  0900001a  bne 0x108bad68
108bad40  0410a0e1  cpy r1, r4
108bad44  0500a0e1  cpy r0, r5
108bad48  d465feeb  bl 0x108544a0
108bad4c  b201d0e1  ldrh r0, [r0,#0x12]
108bad50  0610a0e3  mov r1, #0x6
108bad54  8648feeb  bl 0x1084cf74
108bad58  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bad5c  0010a0e1  cpy r1, r0
108bad60  3100a0e3  mov r0, #0x31
108bad64  daaafeea  b 0x108658d4
108bad68  7080bde8  ldmia sp!,{r4,r5,r6,pc}
