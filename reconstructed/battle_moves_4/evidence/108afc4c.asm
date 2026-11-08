108afc4c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108afc50  0150a0e1  cpy r5, r1
108afc54  0240a0e1  cpy r4, r2
108afc58  0300a0e3  mov r0, #0x3
108afc5c  2eeffeeb  bl 0x1086b91c
108afc60  040050e1  cmp r0, r4
108afc64  0b00001a  bne 0x108afc98
108afc68  0500a0e1  cpy r0, r5
108afc6c  7970feeb  bl 0x1084be58
108afc70  010050e3  cmp r0, #0x1
108afc74  0700001a  bne 0x108afc98
108afc78  2100a0e3  mov r0, #0x21
108afc7c  26effeeb  bl 0x1086b91c
108afc80  010050e3  cmp r0, #0x1
108afc84  0010a0e1  cpy r1, r0
108afc88  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108afc8c  0210a003  moveq r1, #0x2
108afc90  2100a0e3  mov r0, #0x21
108afc94  0ed7feea  b 0x108658d4
108afc98  7080bde8  ldmia sp!,{r4,r5,r6,pc}
