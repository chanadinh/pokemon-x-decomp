108aaa48  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108aaa4c  0160a0e1  cpy r6, r1
108aaa50  0070a0e1  cpy r7, r0
108aaa54  0240a0e1  cpy r4, r2
108aaa58  0350a0e1  cpy r5, r3
108aaa5c  0200a0e3  mov r0, #0x2
108aaa60  ad03ffeb  bl 0x1086b91c
108aaa64  040050e1  cmp r0, r4
108aaa68  0a00001a  bne 0x108aaa98
108aaa6c  000095e5  ldr r0, [r5,#0x0]
108aaa70  000050e3  cmp r0, #0x0
108aaa74  0700000a  beq 0x108aaa98
108aaa78  0700a0e1  cpy r0, r7
108aaa7c  ba87feeb  bl 0x1084c96c
108aaa80  0420a0e1  cpy r2, r4
108aaa84  0010a0e3  mov r1, #0x0
108aaa88  0600a0e1  cpy r0, r6
108aaa8c  ad87feeb  bl 0x1084c948
108aaa90  0000a0e3  mov r0, #0x0
108aaa94  000085e5  str r0, [r5,#0x0]
108aaa98  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
