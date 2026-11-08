108c297c  10402de9  stmdb sp!,{r4,lr}
108c2980  0240a0e1  cpy r4, r2
108c2984  0300a0e3  mov r0, #0x3
108c2988  e3a3feeb  bl 0x1086b91c
108c298c  040050e1  cmp r0, r4
108c2990  0900001a  bne 0x108c29bc
108c2994  1600a0e3  mov r0, #0x16
108c2998  dfa3feeb  bl 0x1086b91c
108c299c  020050e3  cmp r0, #0x2
108c29a0  0500001a  bne 0x108c29bc
108c29a4  1900a0e3  mov r0, #0x19
108c29a8  dba3feeb  bl 0x1086b91c
108c29ac  011080e2  add r1, r0, #0x1
108c29b0  1040bde8  ldmia sp!,{r4,lr}
108c29b4  1900a0e3  mov r0, #0x19
108c29b8  c58bfeea  b 0x108658d4
108c29bc  1080bde8  ldmia sp!,{r4,pc}
