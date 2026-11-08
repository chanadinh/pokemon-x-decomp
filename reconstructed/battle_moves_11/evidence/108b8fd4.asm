108b8fd4  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b8fd8  0240a0e1  cpy r4, r2
108b8fdc  0200a0e3  mov r0, #0x2
108b8fe0  4dcafeeb  bl 0x1086b91c
108b8fe4  040050e1  cmp r0, r4
108b8fe8  1000001a  bne 0x108b9030
108b8fec  4000a0e3  mov r0, #0x40
108b8ff0  49cafeeb  bl 0x1086b91c
108b8ff4  0050a0e1  cpy r5, r0
108b8ff8  1200a0e3  mov r0, #0x12
108b8ffc  46cafeeb  bl 0x1086b91c
108b9000  7040ffe6  uxth r4, r0
108b9004  7f20a0e3  mov r2, #0x7f
108b9008  0110a0e3  mov r1, #0x1
108b900c  0500a0e1  cpy r0, r5
108b9010  af50feeb  bl 0x1084d2d4
108b9014  0410a0e1  cpy r1, r4
108b9018  0500a0e1  cpy r0, r5
108b901c  9e50feeb  bl 0x1084d29c
108b9020  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b9024  0110a0e3  mov r1, #0x1
108b9028  5300a0e3  mov r0, #0x53
108b902c  28b2feea  b 0x108658d4
108b9030  7080bde8  ldmia sp!,{r4,r5,r6,pc}
