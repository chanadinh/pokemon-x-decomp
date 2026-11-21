108b8c84  10402de9  stmdb sp!,{r4,lr}
108b8c88  0240a0e1  cpy r4, r2
108b8c8c  0200a0e3  mov r0, #0x2
108b8c90  21cbfeeb  bl 0x1086b91c
108b8c94  040050e1  cmp r0, r4
108b8c98  0800001a  bne 0x108b8cc0
108b8c9c  2300a0e3  mov r0, #0x23
108b8ca0  1dcbfeeb  bl 0x1086b91c
108b8ca4  ff0000e2  and r0, r0, #0xff
108b8ca8  020050e3  cmp r0, #0x2
108b8cac  0300001a  bne 0x108b8cc0
108b8cb0  1040bde8  ldmia sp!,{r4,lr}
108b8cb4  0110a0e3  mov r1, #0x1
108b8cb8  4200a0e3  mov r0, #0x42
108b8cbc  04b3feea  b 0x108658d4
108b8cc0  1080bde8  ldmia sp!,{r4,pc}
