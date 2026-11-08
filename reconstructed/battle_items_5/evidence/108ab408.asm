108ab408  10402de9  stmdb sp!,{r4,lr}
108ab40c  0240a0e1  cpy r4, r2
108ab410  0200a0e3  mov r0, #0x2
108ab414  4001ffeb  bl 0x1086b91c
108ab418  040050e1  cmp r0, r4
108ab41c  0300001a  bne 0x108ab430
108ab420  1040bde8  ldmia sp!,{r4,lr}
108ab424  0110a0e3  mov r1, #0x1
108ab428  4200a0e3  mov r0, #0x42
108ab42c  28e9feea  b 0x108658d4
108ab430  1080bde8  ldmia sp!,{r4,pc}
