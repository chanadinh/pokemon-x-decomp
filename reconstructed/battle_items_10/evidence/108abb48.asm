108abb48  70402de9  stmdb sp!,{r4,r5,r6,lr}
108abb4c  0240a0e1  cpy r4, r2
108abb50  0350a0e1  cpy r5, r3
108abb54  0300a0e3  mov r0, #0x3
108abb58  6ffffeeb  bl 0x1086b91c
108abb5c  040050e1  cmp r0, r4
108abb60  0a00001a  bne 0x108abb90
108abb64  1200a0e3  mov r0, #0x12
108abb68  6bfffeeb  bl 0x1086b91c
108abb6c  011c40e2  sub r1, r0, #0x100
108abb70  fb1051e2  subs r1, r1, #0xfb
108abb74  0500000a  beq 0x108abb90
108abb78  0110a0e3  mov r1, #0x1
108abb7c  5300a0e3  mov r0, #0x53
108abb80  53e7feeb  bl 0x108658d4
108abb84  000050e3  cmp r0, #0x0
108abb88  0100a013  movne r0, #0x1
108abb8c  00008515  strne r0, [r5,#0x0]
108abb90  7080bde8  ldmia sp!,{r4,r5,r6,pc}
