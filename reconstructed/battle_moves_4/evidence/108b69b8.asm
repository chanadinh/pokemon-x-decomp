108b69b8  10402de9  stmdb sp!,{r4,lr}
108b69bc  0240a0e1  cpy r4, r2
108b69c0  0400a0e3  mov r0, #0x4
108b69c4  d4d3feeb  bl 0x1086b91c
108b69c8  040050e1  cmp r0, r4
108b69cc  0700001a  bne 0x108b69f0
108b69d0  0300a0e3  mov r0, #0x3
108b69d4  d0d3feeb  bl 0x1086b91c
108b69d8  040050e1  cmp r0, r4
108b69dc  0300001a  bne 0x108b69f0
108b69e0  1040bde8  ldmia sp!,{r4,lr}
108b69e4  0a10a0e3  mov r1, #0xa
108b69e8  2000a0e3  mov r0, #0x20
108b69ec  b8bbfeea  b 0x108658d4
108b69f0  1080bde8  ldmia sp!,{r4,pc}
