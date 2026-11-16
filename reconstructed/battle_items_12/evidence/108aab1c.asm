108aab1c  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108aab20  0040a0e1  cpy r4, r0
108aab24  0160a0e1  cpy r6, r1
108aab28  0250a0e1  cpy r5, r2
108aab2c  0200a0e3  mov r0, #0x2
108aab30  7903ffeb  bl 0x1086b91c
108aab34  050050e1  cmp r0, r5
108aab38  1400001a  bne 0x108aab90
108aab3c  0510a0e1  cpy r1, r5
108aab40  0600a0e1  cpy r0, r6
108aab44  55a6feeb  bl 0x108544a0
108aab48  0400a0e1  cpy r0, r4
108aab4c  0370a0e3  mov r7, #0x3
108aab50  b803d0e1  ldrh r0, [r0,#0x38]
108aab54  0710a0e1  cpy r1, r7
108aab58  0589feeb  bl 0x1084cf74
108aab5c  ff2000e2  and r2, r0, #0xff
108aab60  0510a0e1  cpy r1, r5
108aab64  0600a0e1  cpy r0, r6
108aab68  f086feeb  bl 0x1084c730
108aab6c  000050e3  cmp r0, #0x0
108aab70  0600000a  beq 0x108aab90
108aab74  0400a0e1  cpy r0, r4
108aab78  7b87feeb  bl 0x1084c96c
108aab7c  0520a0e1  cpy r2, r5
108aab80  0600a0e1  cpy r0, r6
108aab84  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108aab88  0010a0e3  mov r1, #0x0
108aab8c  6d87feea  b 0x1084c948
108aab90  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
