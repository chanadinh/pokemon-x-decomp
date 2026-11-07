108b44c8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b44cc  0150a0e1  cpy r5, r1
108b44d0  0240a0e1  cpy r4, r2
108b44d4  0300a0e3  mov r0, #0x3
108b44d8  0fddfeeb  bl 0x1086b91c
108b44dc  040050e1  cmp r0, r4
108b44e0  0e00001a  bne 0x108b4520
108b44e4  0400a0e3  mov r0, #0x4
108b44e8  0bddfeeb  bl 0x1086b91c
108b44ec  ff1000e2  and r1, r0, #0xff
108b44f0  0500a0e1  cpy r0, r5
108b44f4  e97ffeeb  bl 0x108544a0
108b44f8  0110a0e3  mov r1, #0x1
108b44fc  c560feeb  bl 0x1084c818
108b4500  000050e3  cmp r0, #0x0
108b4504  0500000a  beq 0x108b4520
108b4508  3100a0e3  mov r0, #0x31
108b450c  02ddfeeb  bl 0x1086b91c
108b4510  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b4514  8010a0e1  mov r1, r0, lsl #0x1
108b4518  3100a0e3  mov r0, #0x31
108b451c  ecc4feea  b 0x108658d4
108b4520  7080bde8  ldmia sp!,{r4,r5,r6,pc}
