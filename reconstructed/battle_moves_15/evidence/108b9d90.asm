108b9d90  10402de9  stmdb sp!,{r4,lr}
108b9d94  0240a0e1  cpy r4, r2
108b9d98  0300a0e3  mov r0, #0x3
108b9d9c  dec6feeb  bl 0x1086b91c
108b9da0  040050e1  cmp r0, r4
108b9da4  0300001a  bne 0x108b9db8
108b9da8  1040bde8  ldmia sp!,{r4,lr}
108b9dac  0110a0e3  mov r1, #0x1
108b9db0  5300a0e3  mov r0, #0x53
108b9db4  c6aefeea  b 0x108658d4
108b9db8  1080bde8  ldmia sp!,{r4,pc}
