108bb520  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bb524  0150a0e1  cpy r5, r1
108bb528  0240a0e1  cpy r4, r2
108bb52c  0300a0e3  mov r0, #0x3
108bb530  f9c0feeb  bl 0x1086b91c
108bb534  040050e1  cmp r0, r4
108bb538  0700001a  bne 0x108bb55c
108bb53c  0500a0e1  cpy r0, r5
108bb540  4442feeb  bl 0x1084be58
108bb544  020050e3  cmp r0, #0x2
108bb548  0300001a  bne 0x108bb55c
108bb54c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bb550  0110a0e3  mov r1, #0x1
108bb554  5300a0e3  mov r0, #0x53
108bb558  dda8feea  b 0x108658d4
108bb55c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
