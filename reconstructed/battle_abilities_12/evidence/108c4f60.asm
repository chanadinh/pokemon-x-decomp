108c4f60  10402de9  stmdb sp!,{r4,lr}
108c4f64  0240a0e1  cpy r4, r2
108c4f68  0300a0e3  mov r0, #0x3
108c4f6c  6a9afeeb  bl 0x1086b91c
108c4f70  040050e1  cmp r0, r4
108c4f74  0900001a  bne 0x108c4fa0
108c4f78  1200a0e3  mov r0, #0x12
108c4f7c  669afeeb  bl 0x1086b91c
108c4f80  7000ffe6  uxth r0, r0
108c4f84  011c40e2  sub r1, r0, #0x100
108c4f88  f91051e2  subs r1, r1, #0xf9
108c4f8c  0300001a  bne 0x108c4fa0
108c4f90  1040bde8  ldmia sp!,{r4,lr}
108c4f94  031ba0e3  mov r1, #0xc00
108c4f98  3600a0e3  mov r0, #0x36
108c4f9c  4c82feea  b 0x108658d4
108c4fa0  1080bde8  ldmia sp!,{r4,pc}
