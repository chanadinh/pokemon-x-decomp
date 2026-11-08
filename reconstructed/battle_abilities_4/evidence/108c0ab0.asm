108c0ab0  10402de9  stmdb sp!,{r4,lr}
108c0ab4  0240a0e1  cpy r4, r2
108c0ab8  0300a0e3  mov r0, #0x3
108c0abc  96abfeeb  bl 0x1086b91c
108c0ac0  040050e1  cmp r0, r4
108c0ac4  0600001a  bne 0x108c0ae4
108c0ac8  2700a0e3  mov r0, #0x27
108c0acc  92abfeeb  bl 0x1086b91c
108c0ad0  10109fe5  ldr r1, [0x108c0ae8]
108c0ad4  1040bde8  ldmia sp!,{r4,lr}
108c0ad8  801001e0  and r1, r1, r0, lsl #0x1
108c0adc  2700a0e3  mov r0, #0x27
108c0ae0  7b93feea  b 0x108658d4
108c0ae4  1080bde8  ldmia sp!,{r4,pc}
