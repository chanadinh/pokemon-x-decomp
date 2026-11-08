108bea04  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bea08  0350a0e1  cpy r5, r3
108bea0c  1200a0e3  mov r0, #0x12
108bea10  c1b3feeb  bl 0x1086b91c
108bea14  7040ffe6  uxth r4, r0
108bea18  990054e3  cmp r4, #0x99
108bea1c  0010a0e3  mov r1, #0x0
108bea20  78005413  cmpne r4, #0x78
108bea24  001085e5  str r1, [r5,#0x0]
108bea28  0700001a  bne 0x108bea4c
108bea2c  2300a0e3  mov r0, #0x23
108bea30  b9b3feeb  bl 0x1086b91c
108bea34  ff0010e3  tst r0, #0xff
108bea38  0300001a  bne 0x108bea4c
108bea3c  1310a0e3  mov r1, #0x13
108bea40  2300a0e3  mov r0, #0x23
108bea44  a29bfeeb  bl 0x108658d4
108bea48  110085e8  stmia r5,{r0,r4}
108bea4c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
