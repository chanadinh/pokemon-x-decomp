108aa5c4  10402de9  stmdb sp!,{r4,lr}
108aa5c8  0240a0e1  cpy r4, r2
108aa5cc  0300a0e3  mov r0, #0x3
108aa5d0  d104ffeb  bl 0x1086b91c
108aa5d4  040050e1  cmp r0, r4
108aa5d8  0900001a  bne 0x108aa604
108aa5dc  1200a0e3  mov r0, #0x12
108aa5e0  cd04ffeb  bl 0x1086b91c
108aa5e4  7000ffe6  uxth r0, r0
108aa5e8  7b58fdeb  bl 0x108007dc
108aa5ec  020050e3  cmp r0, #0x2
108aa5f0  0300001a  bne 0x108aa604
108aa5f4  1040bde8  ldmia sp!,{r4,lr}
108aa5f8  061ba0e3  mov r1, #0x1800
108aa5fc  3600a0e3  mov r0, #0x36
108aa600  1f8afeea  b 0x1084ce84
108aa604  1080bde8  ldmia sp!,{r4,pc}
