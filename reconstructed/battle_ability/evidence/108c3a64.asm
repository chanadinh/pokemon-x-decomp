108c3a64  10402de9  stmdb sp!,{r4,lr}
108c3a68  0240a0e1  cpy r4, r2
108c3a6c  0300a0e3  mov r0, #0x3
108c3a70  a99ffeeb  bl 0x1086b91c
108c3a74  040050e1  cmp r0, r4
108c3a78  0d00001a  bne 0x108c3ab4
108c3a7c  1200a0e3  mov r0, #0x12
108c3a80  a59ffeeb  bl 0x1086b91c
108c3a84  7000ffe6  uxth r0, r0
108c3a88  53f3fceb  bl 0x108007dc
108c3a8c  010050e3  cmp r0, #0x1
108c3a90  0700001a  bne 0x108c3ab4
108c3a94  3400a0e3  mov r0, #0x34
108c3a98  9f9ffeeb  bl 0x1086b91c
108c3a9c  061ba0e3  mov r1, #0x1800
108c3aa0  186bffeb  bl 0x1089e708
108c3aa4  0010a0e1  cpy r1, r0
108c3aa8  1040bde8  ldmia sp!,{r4,lr}
108c3aac  3400a0e3  mov r0, #0x34
108c3ab0  8787feea  b 0x108658d4
108c3ab4  1080bde8  ldmia sp!,{r4,pc}
