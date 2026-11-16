108adce8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108adcec  0050a0e1  cpy r5, r0
108adcf0  0240a0e1  cpy r4, r2
108adcf4  0300a0e3  mov r0, #0x3
108adcf8  07f7feeb  bl 0x1086b91c
108adcfc  040050e1  cmp r0, r4
108add00  0700001a  bne 0x108add24
108add04  0500a0e1  cpy r0, r5
108add08  b803d0e1  ldrh r0, [r0,#0x38]
108add0c  2e0050e3  cmp r0, #0x2e
108add10  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108add14  5900a0e3  mov r0, #0x59
108add18  08109f05  ldreq r1, [0x108add28]
108add1c  08109f15  ldrne r1, [0x108add2c]
108add20  ebdefeea  b 0x108658d4
108add24  7080bde8  ldmia sp!,{r4,r5,r6,pc}
