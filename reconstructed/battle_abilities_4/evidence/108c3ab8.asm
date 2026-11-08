108c3ab8  10402de9  stmdb sp!,{r4,lr}
108c3abc  0240a0e1  cpy r4, r2
108c3ac0  0300a0e3  mov r0, #0x3
108c3ac4  949ffeeb  bl 0x1086b91c
108c3ac8  040050e1  cmp r0, r4
108c3acc  0900001a  bne 0x108c3af8
108c3ad0  1200a0e3  mov r0, #0x12
108c3ad4  909ffeeb  bl 0x1086b91c
108c3ad8  7000ffe6  uxth r0, r0
108c3adc  3ef3fceb  bl 0x108007dc
108c3ae0  010050e3  cmp r0, #0x1
108c3ae4  0300001a  bne 0x108c3af8
108c3ae8  0c109fe5  ldr r1, [0x108c3afc]
108c3aec  1040bde8  ldmia sp!,{r4,lr}
108c3af0  3600a0e3  mov r0, #0x36
108c3af4  e224feea  b 0x1084ce84
108c3af8  1080bde8  ldmia sp!,{r4,pc}
