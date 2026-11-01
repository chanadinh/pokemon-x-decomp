108bf95c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bf960  0150a0e1  cpy r5, r1
108bf964  0240a0e1  cpy r4, r2
108bf968  0300a0e3  mov r0, #0x3
108bf96c  eaaffeeb  bl 0x1086b91c
108bf970  040050e1  cmp r0, r4
108bf974  1f00001a  bne 0x108bf9f8
108bf978  0410a0e1  cpy r1, r4
108bf97c  0500a0e1  cpy r0, r5
108bf980  c652feeb  bl 0x108544a0
108bf984  0040a0e1  cpy r4, r0
108bf988  0400a0e3  mov r0, #0x4
108bf98c  e2affeeb  bl 0x1086b91c
108bf990  ff1000e2  and r1, r0, #0xff
108bf994  0500a0e1  cpy r0, r5
108bf998  c052feeb  bl 0x108544a0
108bf99c  0050a0e1  cpy r5, r0
108bf9a0  1210a0e3  mov r1, #0x12
108bf9a4  0400a0e1  cpy r0, r4
108bf9a8  9cd2feeb  bl 0x10874420
108bf9ac  ff4000e2  and r4, r0, #0xff
108bf9b0  1210a0e3  mov r1, #0x12
108bf9b4  0500a0e1  cpy r0, r5
108bf9b8  98d2feeb  bl 0x10874420
108bf9bc  020054e3  cmp r4, #0x2
108bf9c0  ff0000e2  and r0, r0, #0xff
108bf9c4  0b00000a  beq 0x108bf9f8
108bf9c8  020050e3  cmp r0, #0x2
108bf9cc  0900000a  beq 0x108bf9f8
108bf9d0  000054e1  cmp r4, r0
108bf9d4  0300001a  bne 0x108bf9e8
108bf9d8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bf9dc  051ba0e3  mov r1, #0x1400
108bf9e0  3200a0e3  mov r0, #0x32
108bf9e4  2635feea  b 0x1084ce84
108bf9e8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bf9ec  031ba0e3  mov r1, #0xc00
108bf9f0  3200a0e3  mov r0, #0x32
108bf9f4  2235feea  b 0x1084ce84
108bf9f8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
