108b9cb0  10402de9  stmdb sp!,{r4,lr}
108b9cb4  0240a0e1  cpy r4, r2
108b9cb8  0300a0e3  mov r0, #0x3
108b9cbc  16c7feeb  bl 0x1086b91c
108b9cc0  040050e1  cmp r0, r4
108b9cc4  0300001a  bne 0x108b9cd8
108b9cc8  1040bde8  ldmia sp!,{r4,lr}
108b9ccc  0010a0e3  mov r1, #0x0
108b9cd0  5000a0e3  mov r0, #0x50
108b9cd4  feaefeea  b 0x108658d4
108b9cd8  1080bde8  ldmia sp!,{r4,pc}
