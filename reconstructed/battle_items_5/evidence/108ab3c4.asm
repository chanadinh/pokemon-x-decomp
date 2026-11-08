108ab3c4  10402de9  stmdb sp!,{r4,lr}
108ab3c8  0240a0e1  cpy r4, r2
108ab3cc  0300a0e3  mov r0, #0x3
108ab3d0  5101ffeb  bl 0x1086b91c
108ab3d4  040050e1  cmp r0, r4
108ab3d8  0900001a  bne 0x108ab404
108ab3dc  1200a0e3  mov r0, #0x12
108ab3e0  4d01ffeb  bl 0x1086b91c
108ab3e4  7000ffe6  uxth r0, r0
108ab3e8  fb54fdeb  bl 0x108007dc
108ab3ec  010050e3  cmp r0, #0x1
108ab3f0  0300001a  bne 0x108ab404
108ab3f4  1040bde8  ldmia sp!,{r4,lr}
108ab3f8  061ba0e3  mov r1, #0x1800
108ab3fc  3600a0e3  mov r0, #0x36
108ab400  9f86feea  b 0x1084ce84
108ab404  1080bde8  ldmia sp!,{r4,pc}
