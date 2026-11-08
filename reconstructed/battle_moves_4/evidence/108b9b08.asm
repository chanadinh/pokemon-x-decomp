108b9b08  10402de9  stmdb sp!,{r4,lr}
108b9b0c  0240a0e1  cpy r4, r2
108b9b10  0300a0e3  mov r0, #0x3
108b9b14  80c7feeb  bl 0x1086b91c
108b9b18  040050e1  cmp r0, r4
108b9b1c  0700001a  bne 0x108b9b40
108b9b20  2200a0e3  mov r0, #0x22
108b9b24  7cc7feeb  bl 0x1086b91c
108b9b28  040050e3  cmp r0, #0x4
108b9b2c  0300001a  bne 0x108b9b40
108b9b30  1040bde8  ldmia sp!,{r4,lr}
108b9b34  0010a0e3  mov r1, #0x0
108b9b38  4300a0e3  mov r0, #0x43
108b9b3c  64affeea  b 0x108658d4
108b9b40  1080bde8  ldmia sp!,{r4,pc}
