108b5b88  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b5b8c  0150a0e1  cpy r5, r1
108b5b90  0240a0e1  cpy r4, r2
108b5b94  0300a0e3  mov r0, #0x3
108b5b98  5fd7feeb  bl 0x1086b91c
108b5b9c  040050e1  cmp r0, r4
108b5ba0  0d00001a  bne 0x108b5bdc
108b5ba4  0400a0e3  mov r0, #0x4
108b5ba8  5bd7feeb  bl 0x1086b91c
108b5bac  ff1000e2  and r1, r0, #0xff
108b5bb0  0500a0e1  cpy r0, r5
108b5bb4  397afeeb  bl 0x108544a0
108b5bb8  0d10a0e3  mov r1, #0xd
108b5bbc  17fafeeb  bl 0x10874420
108b5bc0  a00f80e0  add r0, r0, r0, lsr #0x1f
108b5bc4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b5bc8  8007a0e1  mov r0, r0, lsl #0xf
108b5bcc  2018b0e1  movs r1, r0, lsr #0x10
108b5bd0  0110a003  moveq r1, #0x1
108b5bd4  3800a0e3  mov r0, #0x38
108b5bd8  3dbffeea  b 0x108658d4
108b5bdc  7080bde8  ldmia sp!,{r4,r5,r6,pc}
