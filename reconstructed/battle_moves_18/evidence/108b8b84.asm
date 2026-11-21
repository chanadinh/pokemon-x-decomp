108b8b84  10402de9  stmdb sp!,{r4,lr}
108b8b88  0240a0e1  cpy r4, r2
108b8b8c  0300a0e3  mov r0, #0x3
108b8b90  61cbfeeb  bl 0x1086b91c
108b8b94  040050e1  cmp r0, r4
108b8b98  0c00001a  bne 0x108b8bd0
108b8b9c  0400a0e3  mov r0, #0x4
108b8ba0  5dcbfeeb  bl 0x1086b91c
108b8ba4  ff1000e2  and r1, r0, #0xff
108b8ba8  1f0051e3  cmp r1, #0x1f
108b8bac  0700000a  beq 0x108b8bd0
108b8bb0  0400a0e1  cpy r0, r4
108b8bb4  7b4efeeb  bl 0x1084c5a8
108b8bb8  000050e3  cmp r0, #0x0
108b8bbc  0300000a  beq 0x108b8bd0
108b8bc0  1040bde8  ldmia sp!,{r4,lr}
108b8bc4  0110a0e3  mov r1, #0x1
108b8bc8  4200a0e3  mov r0, #0x42
108b8bcc  40b3feea  b 0x108658d4
108b8bd0  1080bde8  ldmia sp!,{r4,pc}
