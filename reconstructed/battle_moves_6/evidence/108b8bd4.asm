108b8bd4  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b8bd8  0160a0e1  cpy r6, r1
108b8bdc  0250a0e1  cpy r5, r2
108b8be0  0300a0e3  mov r0, #0x3
108b8be4  4ccbfeeb  bl 0x1086b91c
108b8be8  050050e1  cmp r0, r5
108b8bec  0f00001a  bne 0x108b8c30
108b8bf0  0520a0e1  cpy r2, r5
108b8bf4  0310a0e3  mov r1, #0x3
108b8bf8  0600a0e1  cpy r0, r6
108b8bfc  dec4feeb  bl 0x10869f7c
108b8c00  0040a0e1  cpy r4, r0
108b8c04  28209fe5  ldr r2, [0x108b8c34]
108b8c08  0210a0e3  mov r1, #0x2
108b8c0c  040080e2  add r0, r0, #0x4
108b8c10  af51feeb  bl 0x1084d2d4
108b8c14  0510a0e1  cpy r1, r5
108b8c18  040084e2  add r0, r4, #0x4
108b8c1c  9e51feeb  bl 0x1084d29c
108b8c20  0410a0e1  cpy r1, r4
108b8c24  0600a0e1  cpy r0, r6
108b8c28  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b8c2c  c5bbfeea  b 0x10867b48
108b8c30  7080bde8  ldmia sp!,{r4,r5,r6,pc}
