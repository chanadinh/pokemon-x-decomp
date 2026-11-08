108bdd94  10402de9  stmdb sp!,{r4,lr}
108bdd98  0240a0e1  cpy r4, r2
108bdd9c  0200a0e3  mov r0, #0x2
108bdda0  ddb6feeb  bl 0x1086b91c
108bdda4  ff0000e2  and r0, r0, #0xff
108bdda8  0410a0e1  cpy r1, r4
108bddac  fd39feeb  bl 0x1084c5a8
108bddb0  000050e3  cmp r0, #0x0
108bddb4  0300000a  beq 0x108bddc8
108bddb8  1040bde8  ldmia sp!,{r4,lr}
108bddbc  0110a0e3  mov r1, #0x1
108bddc0  5300a0e3  mov r0, #0x53
108bddc4  c29efeea  b 0x108658d4
108bddc8  1080bde8  ldmia sp!,{r4,pc}
