108ab0d4  10402de9  stmdb sp!,{r4,lr}
108ab0d8  0240a0e1  cpy r4, r2
108ab0dc  0200a0e3  mov r0, #0x2
108ab0e0  0d02ffeb  bl 0x1086b91c
108ab0e4  040050e1  cmp r0, r4
108ab0e8  0300001a  bne 0x108ab0fc
108ab0ec  1040bde8  ldmia sp!,{r4,lr}
108ab0f0  021ba0e3  mov r1, #0x800
108ab0f4  3600a0e3  mov r0, #0x36
108ab0f8  6187feea  b 0x1084ce84
108ab0fc  1080bde8  ldmia sp!,{r4,pc}
