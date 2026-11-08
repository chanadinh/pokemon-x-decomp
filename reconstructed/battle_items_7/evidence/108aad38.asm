108aad38  70402de9  stmdb sp!,{r4,r5,r6,lr}
108aad3c  0060a0e1  cpy r6, r0
108aad40  0150a0e1  cpy r5, r1
108aad44  0240a0e1  cpy r4, r2
108aad48  0200a0e3  mov r0, #0x2
108aad4c  f202ffeb  bl 0x1086b91c
108aad50  040050e1  cmp r0, r4
108aad54  1300001a  bne 0x108aada8
108aad58  0410a0e1  cpy r1, r4
108aad5c  0500a0e1  cpy r0, r5
108aad60  cea5feeb  bl 0x108544a0
108aad64  6b89feeb  bl 0x1084d318
108aad68  000050e3  cmp r0, #0x0
108aad6c  0d00001a  bne 0x108aada8
108aad70  0420a0e1  cpy r2, r4
108aad74  0010a0e3  mov r1, #0x0
108aad78  0500a0e1  cpy r0, r5
108aad7c  7efcfeeb  bl 0x10869f7c
108aad80  0040a0e1  cpy r4, r0
108aad84  040090e5  ldr r0, [r0,#0x4]
108aad88  010080e3  orr r0, r0, #0x1
108aad8c  040084e5  str r0, [r4,#0x4]
108aad90  0600a0e1  cpy r0, r6
108aad94  f486feeb  bl 0x1084c96c
108aad98  0410a0e1  cpy r1, r4
108aad9c  0500a0e1  cpy r0, r5
108aada0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108aada4  67f3feea  b 0x10867b48
108aada8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
