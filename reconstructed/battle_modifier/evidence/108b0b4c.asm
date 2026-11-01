108b0b4c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b0b50  0150a0e1  cpy r5, r1
108b0b54  0240a0e1  cpy r4, r2
108b0b58  0300a0e3  mov r0, #0x3
108b0b5c  6eebfeeb  bl 0x1086b91c
108b0b60  040050e1  cmp r0, r4
108b0b64  0c00001a  bne 0x108b0b9c
108b0b68  0400a0e3  mov r0, #0x4
108b0b6c  6aebfeeb  bl 0x1086b91c
108b0b70  ff1000e2  and r1, r0, #0xff
108b0b74  0500a0e1  cpy r0, r5
108b0b78  488efeeb  bl 0x108544a0
108b0b7c  0410a0e3  mov r1, #0x4
108b0b80  b16ffeeb  bl 0x1084ca4c
108b0b84  000050e3  cmp r0, #0x0
108b0b88  0300000a  beq 0x108b0b9c
108b0b8c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b0b90  021aa0e3  mov r1, #0x2000
108b0b94  3600a0e3  mov r0, #0x36
108b0b98  b970feea  b 0x1084ce84
108b0b9c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
