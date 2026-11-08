108abb94  70402de9  stmdb sp!,{r4,r5,r6,lr}
108abb98  0150a0e1  cpy r5, r1
108abb9c  0240a0e1  cpy r4, r2
108abba0  0200a0e3  mov r0, #0x2
108abba4  5cfffeeb  bl 0x1086b91c
108abba8  040050e1  cmp r0, r4
108abbac  0d00001a  bne 0x108abbe8
108abbb0  0420a0e1  cpy r2, r4
108abbb4  0a10a0e3  mov r1, #0xa
108abbb8  0500a0e1  cpy r0, r5
108abbbc  eef8feeb  bl 0x10869f7c
108abbc0  0010a0e1  cpy r1, r0
108abbc4  1b00a0e3  mov r0, #0x1b
108abbc8  0400c1e5  strb r0, [r1,#0x4]
108abbcc  0120a0e3  mov r2, #0x1
108abbd0  1120c1e5  strb r2, [r1,#0x11]
108abbd4  1220c1e5  strb r2, [r1,#0x12]
108abbd8  0500a0e1  cpy r0, r5
108abbdc  0540c1e5  strb r4, [r1,#0x5]
108abbe0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108abbe4  d7effeea  b 0x10867b48
108abbe8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
