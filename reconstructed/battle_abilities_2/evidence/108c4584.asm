108c4584  10402de9  stmdb sp!,{r4,lr}
108c4588  0240a0e1  cpy r4, r2
108c458c  0200a0e3  mov r0, #0x2
108c4590  e19cfeeb  bl 0x1086b91c
108c4594  040050e1  cmp r0, r4
108c4598  0b00001a  bne 0x108c45cc
108c459c  3300a0e3  mov r0, #0x33
108c45a0  dd9cfeeb  bl 0x1086b91c
108c45a4  000050e3  cmp r0, #0x0
108c45a8  070000da  ble 0x108c45cc
108c45ac  3a00a0e3  mov r0, #0x3a
108c45b0  d99cfeeb  bl 0x1086b91c
108c45b4  040050e3  cmp r0, #0x4
108c45b8  0300001a  bne 0x108c45cc
108c45bc  1040bde8  ldmia sp!,{r4,lr}
108c45c0  0110a0e3  mov r1, #0x1
108c45c4  4200a0e3  mov r0, #0x42
108c45c8  c184feea  b 0x108658d4
108c45cc  1080bde8  ldmia sp!,{r4,pc}
