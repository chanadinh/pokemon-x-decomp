108b3ad8  38402de9  stmdb sp!,{r3,r4,r5,lr}
108b3adc  0240a0e1  cpy r4, r2
108b3ae0  0300a0e3  mov r0, #0x3
108b3ae4  8cdffeeb  bl 0x1086b91c
108b3ae8  040050e1  cmp r0, r4
108b3aec  0800001a  bne 0x108b3b14
108b3af0  1f00a0e3  mov r0, #0x1f
108b3af4  88dffeeb  bl 0x1086b91c
108b3af8  00008de5  str r0, [sp,#0x0]
108b3afc  1010a0e3  mov r1, #0x10
108b3b00  0d00a0e1  cpy r0, sp
108b3b04  a15ffeeb  bl 0x1084b990
108b3b08  00109de5  ldr r1, [sp,#0x0]
108b3b0c  1f00a0e3  mov r0, #0x1f
108b3b10  6fc7feeb  bl 0x108658d4
108b3b14  3880bde8  ldmia sp!,{r3,r4,r5,pc}
