108c3d0c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c3d10  0150a0e1  cpy r5, r1
108c3d14  0240a0e1  cpy r4, r2
108c3d18  0300a0e3  mov r0, #0x3
108c3d1c  fe9efeeb  bl 0x1086b91c
108c3d20  040050e1  cmp r0, r4
108c3d24  0d00001a  bne 0x108c3d60
108c3d28  0500a0e1  cpy r0, r5
108c3d2c  4920feeb  bl 0x1084be58
108c3d30  010050e3  cmp r0, #0x1
108c3d34  0900001a  bne 0x108c3d60
108c3d38  1200a0e3  mov r0, #0x12
108c3d3c  f69efeeb  bl 0x1086b91c
108c3d40  7000ffe6  uxth r0, r0
108c3d44  a4f2fceb  bl 0x108007dc
108c3d48  020050e3  cmp r0, #0x2
108c3d4c  0300001a  bne 0x108c3d60
108c3d50  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c3d54  061ba0e3  mov r1, #0x1800
108c3d58  3600a0e3  mov r0, #0x36
108c3d5c  4824feea  b 0x1084ce84
108c3d60  7080bde8  ldmia sp!,{r4,r5,r6,pc}
