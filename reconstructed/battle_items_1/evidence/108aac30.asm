108aac30  70402de9  stmdb sp!,{r4,r5,r6,lr}
108aac34  0140a0e1  cpy r4, r1
108aac38  0060a0e1  cpy r6, r0
108aac3c  0250a0e1  cpy r5, r2
108aac40  0210a0e1  cpy r1, r2
108aac44  0400a0e1  cpy r0, r4
108aac48  14a6feeb  bl 0x108544a0
108aac4c  0f10a0e3  mov r1, #0xf
108aac50  8caffeeb  bl 0x10856a88
108aac54  000050e3  cmp r0, #0x0
108aac58  0c00001a  bne 0x108aac90
108aac5c  0220a0e3  mov r2, #0x2
108aac60  0510a0e1  cpy r1, r5
108aac64  0400a0e1  cpy r0, r4
108aac68  b086feeb  bl 0x1084c730
108aac6c  000050e3  cmp r0, #0x0
108aac70  0600000a  beq 0x108aac90
108aac74  0600a0e1  cpy r0, r6
108aac78  3b87feeb  bl 0x1084c96c
108aac7c  0520a0e1  cpy r2, r5
108aac80  0400a0e1  cpy r0, r4
108aac84  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108aac88  0010a0e3  mov r1, #0x0
108aac8c  2d87feea  b 0x1084c948
108aac90  7080bde8  ldmia sp!,{r4,r5,r6,pc}
