108b9a54  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b9a58  0150a0e1  cpy r5, r1
108b9a5c  0240a0e1  cpy r4, r2
108b9a60  0200a0e3  mov r0, #0x2
108b9a64  acc7feeb  bl 0x1086b91c
108b9a68  040050e1  cmp r0, r4
108b9a6c  1000001a  bne 0x108b9ab4
108b9a70  0500a0e1  cpy r0, r5
108b9a74  0410a0e1  cpy r1, r4
108b9a78  886afeeb  bl 0x108544a0
108b9a7c  000050e3  cmp r0, #0x0
108b9a80  0200000a  beq 0x108b9a90
108b9a84  000090e5  ldr r0, [r0,#0x0]
108b9a88  1b20fdeb  bl 0x10801afc
108b9a8c  040000ea  b 0x108b9aa4
108b9a90  0020a0e3  mov r2, #0x0
108b9a94  0210a0e1  cpy r1, r2
108b9a98  0200a0e1  cpy r0, r2
108b9a9c  3c1afdeb  bl 0x10800394
108b9aa0  0000a0e3  mov r0, #0x0
108b9aa4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b9aa8  0010a0e1  cpy r1, r0
108b9aac  1600a0e3  mov r0, #0x16
108b9ab0  87affeea  b 0x108658d4
108b9ab4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
