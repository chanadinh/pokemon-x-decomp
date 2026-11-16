108baa30  70402de9  stmdb sp!,{r4,r5,r6,lr}
108baa34  0150a0e1  cpy r5, r1
108baa38  0240a0e1  cpy r4, r2
108baa3c  0400a0e3  mov r0, #0x4
108baa40  b5c3feeb  bl 0x1086b91c
108baa44  040050e1  cmp r0, r4
108baa48  1a00001a  bne 0x108baab8
108baa4c  1200a0e3  mov r0, #0x12
108baa50  b1c3feeb  bl 0x1086b91c
108baa54  7000ffe6  uxth r0, r0
108baa58  0010a0e3  mov r1, #0x0
108baa5c  8e16fdeb  bl 0x1080049c
108baa60  000050e3  cmp r0, #0x0
108baa64  1300000a  beq 0x108baab8
108baa68  0420a0e1  cpy r2, r4
108baa6c  0d10a0e3  mov r1, #0xd
108baa70  0500a0e1  cpy r0, r5
108baa74  40bdfeeb  bl 0x10869f7c
108baa78  0040a0e1  cpy r4, r0
108baa7c  1800d0e5  ldrb r0, [r0,#0x18]
108baa80  0120a0e3  mov r2, #0x1
108baa84  fe30a0e3  mov r3, #0xfe
108baa88  0420c4e5  strb r2, [r4,#0x4]
108baa8c  1730c4e5  strb r3, [r4,#0x17]
108baa90  160080e3  orr r0, r0, #0x16
108baa94  1620c4e5  strb r2, [r4,#0x16]
108baa98  1800c4e5  strb r0, [r4,#0x18]
108baa9c  0300a0e3  mov r0, #0x3
108baaa0  9dc3feeb  bl 0x1086b91c
108baaa4  0c00c4e5  strb r0, [r4,#0xc]
108baaa8  0410a0e1  cpy r1, r4
108baaac  0500a0e1  cpy r0, r5
108baab0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108baab4  23b4feea  b 0x10867b48
108baab8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
