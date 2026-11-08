108bd17c  10402de9  stmdb sp!,{r4,lr}
108bd180  0240a0e1  cpy r4, r2
108bd184  0300a0e3  mov r0, #0x3
108bd188  e3b9feeb  bl 0x1086b91c
108bd18c  040050e1  cmp r0, r4
108bd190  0600001a  bne 0x108bd1b0
108bd194  2d00a0e3  mov r0, #0x2d
108bd198  dfb9feeb  bl 0x1086b91c
108bd19c  010080e2  add r0, r0, #0x1
108bd1a0  ff1000e2  and r1, r0, #0xff
108bd1a4  1040bde8  ldmia sp!,{r4,lr}
108bd1a8  2d00a0e3  mov r0, #0x2d
108bd1ac  c8a1feea  b 0x108658d4
108bd1b0  1080bde8  ldmia sp!,{r4,pc}
