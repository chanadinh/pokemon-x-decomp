108aaa9c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108aaaa0  0060a0e1  cpy r6, r0
108aaaa4  0150a0e1  cpy r5, r1
108aaaa8  0240a0e1  cpy r4, r2
108aaaac  0400a0e3  mov r0, #0x4
108aaab0  9903ffeb  bl 0x1086b91c
108aaab4  040050e1  cmp r0, r4
108aaab8  1600001a  bne 0x108aab18
108aaabc  4700a0e3  mov r0, #0x47
108aaac0  9503ffeb  bl 0x1086b91c
108aaac4  000050e3  cmp r0, #0x0
108aaac8  1200001a  bne 0x108aab18
108aaacc  1600a0e3  mov r0, #0x16
108aaad0  9103ffeb  bl 0x1086b91c
108aaad4  0a0050e3  cmp r0, #0xa
108aaad8  0e00001a  bne 0x108aab18
108aaadc  0410a0e1  cpy r1, r4
108aaae0  0500a0e1  cpy r0, r5
108aaae4  6da6feeb  bl 0x108544a0
108aaae8  0120a0e3  mov r2, #0x1
108aaaec  0310a0e3  mov r1, #0x3
108aaaf0  fc1dffeb  bl 0x108722e8
108aaaf4  000050e3  cmp r0, #0x0
108aaaf8  0600000a  beq 0x108aab18
108aaafc  0600a0e1  cpy r0, r6
108aab00  9987feeb  bl 0x1084c96c
108aab04  0420a0e1  cpy r2, r4
108aab08  0500a0e1  cpy r0, r5
108aab0c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108aab10  0010a0e3  mov r1, #0x0
108aab14  8b87feea  b 0x1084c948
108aab18  7080bde8  ldmia sp!,{r4,r5,r6,pc}
