108beac0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108beac4  0150a0e1  cpy r5, r1
108beac8  0240a0e1  cpy r4, r2
108beacc  0200a0e3  mov r0, #0x2
108bead0  91b3feeb  bl 0x1086b91c
108bead4  040050e1  cmp r0, r4
108bead8  0700001a  bne 0x108beafc
108beadc  0500a0e1  cpy r0, r5
108beae0  dc34feeb  bl 0x1084be58
108beae4  040050e3  cmp r0, #0x4
108beae8  0300001a  bne 0x108beafc
108beaec  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108beaf0  021aa0e3  mov r1, #0x2000
108beaf4  3600a0e3  mov r0, #0x36
108beaf8  e138feea  b 0x1084ce84
108beafc  7080bde8  ldmia sp!,{r4,r5,r6,pc}
