108bd110  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bd114  0150a0e1  cpy r5, r1
108bd118  0240a0e1  cpy r4, r2
108bd11c  0300a0e3  mov r0, #0x3
108bd120  fdb9feeb  bl 0x1086b91c
108bd124  040050e1  cmp r0, r4
108bd128  1200001a  bne 0x108bd178
108bd12c  0410a0e1  cpy r1, r4
108bd130  0500a0e1  cpy r0, r5
108bd134  d95cfeeb  bl 0x108544a0
108bd138  dbc1feeb  bl 0x1086d8ac
108bd13c  000050e3  cmp r0, #0x0
108bd140  0c00000a  beq 0x108bd178
108bd144  1200a0e3  mov r0, #0x12
108bd148  f3b9feeb  bl 0x1086b91c
108bd14c  7000ffe6  uxth r0, r0
108bd150  a10dfdeb  bl 0x108007dc
108bd154  010050e3  cmp r0, #0x1
108bd158  0600001a  bne 0x108bd178
108bd15c  061ba0e3  mov r1, #0x1800
108bd160  3600a0e3  mov r0, #0x36
108bd164  463ffeeb  bl 0x1084ce84
108bd168  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bd16c  0110a0e3  mov r1, #0x1
108bd170  5400a0e3  mov r0, #0x54
108bd174  d6a1feea  b 0x108658d4
108bd178  7080bde8  ldmia sp!,{r4,r5,r6,pc}
