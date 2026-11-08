108b9d28  10402de9  stmdb sp!,{r4,lr}
108b9d2c  0240a0e1  cpy r4, r2
108b9d30  0300a0e3  mov r0, #0x3
108b9d34  f8c6feeb  bl 0x1086b91c
108b9d38  040050e1  cmp r0, r4
108b9d3c  0700001a  bne 0x108b9d60
108b9d40  2200a0e3  mov r0, #0x22
108b9d44  f4c6feeb  bl 0x1086b91c
108b9d48  030050e3  cmp r0, #0x3
108b9d4c  0300001a  bne 0x108b9d60
108b9d50  1040bde8  ldmia sp!,{r4,lr}
108b9d54  0010a0e3  mov r1, #0x0
108b9d58  4300a0e3  mov r0, #0x43
108b9d5c  dcaefeea  b 0x108658d4
108b9d60  1080bde8  ldmia sp!,{r4,pc}
