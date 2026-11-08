108a64bc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a64c0  0150a0e1  cpy r5, r1
108a64c4  0240a0e1  cpy r4, r2
108a64c8  0300a0e3  mov r0, #0x3
108a64cc  1215ffeb  bl 0x1086b91c
108a64d0  040050e1  cmp r0, r4
108a64d4  0900001a  bne 0x108a6500
108a64d8  0410a0e1  cpy r1, r4
108a64dc  0500a0e1  cpy r0, r5
108a64e0  eeb7feeb  bl 0x108544a0
108a64e4  bc00d0e1  ldrh r0, [r0,#0xc]
108a64e8  190050e3  cmp r0, #0x19
108a64ec  0300001a  bne 0x108a6500
108a64f0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a64f4  021aa0e3  mov r1, #0x2000
108a64f8  3600a0e3  mov r0, #0x36
108a64fc  609afeea  b 0x1084ce84
108a6500  7080bde8  ldmia sp!,{r4,r5,r6,pc}
