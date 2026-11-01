108b9c28  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b9c2c  0150a0e1  cpy r5, r1
108b9c30  0240a0e1  cpy r4, r2
108b9c34  0200a0e3  mov r0, #0x2
108b9c38  37c7feeb  bl 0x1086b91c
108b9c3c  040050e1  cmp r0, r4
108b9c40  0900001a  bne 0x108b9c6c
108b9c44  0410a0e1  cpy r1, r4
108b9c48  0500a0e1  cpy r0, r5
108b9c4c  136afeeb  bl 0x108544a0
108b9c50  b201d0e1  ldrh r0, [r0,#0x12]
108b9c54  0810a0e3  mov r1, #0x8
108b9c58  c54cfeeb  bl 0x1084cf74
108b9c5c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b9c60  0010a0e1  cpy r1, r0
108b9c64  1600a0e3  mov r0, #0x16
108b9c68  19affeea  b 0x108658d4
108b9c6c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
