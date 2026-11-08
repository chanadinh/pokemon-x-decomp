108afc9c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108afca0  0150a0e1  cpy r5, r1
108afca4  0240a0e1  cpy r4, r2
108afca8  0300a0e3  mov r0, #0x3
108afcac  1aeffeeb  bl 0x1086b91c
108afcb0  040050e1  cmp r0, r4
108afcb4  0b00001a  bne 0x108afce8
108afcb8  0400a0e3  mov r0, #0x4
108afcbc  16effeeb  bl 0x1086b91c
108afcc0  ff1000e2  and r1, r0, #0xff
108afcc4  0500a0e1  cpy r0, r5
108afcc8  f491feeb  bl 0x108544a0
108afccc  b305ffeb  bl 0x108713a0
108afcd0  320a50e3  cmp r0, #0x32000
108afcd4  030000ca  bgt 0x108afce8
108afcd8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108afcdc  021aa0e3  mov r1, #0x2000
108afce0  3200a0e3  mov r0, #0x32
108afce4  6674feea  b 0x1084ce84
108afce8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
