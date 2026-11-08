108a7af4  38402de9  stmdb sp!,{r3,r4,r5,lr}
108a7af8  0240a0e1  cpy r4, r2
108a7afc  0300a0e3  mov r0, #0x3
108a7b00  850fffeb  bl 0x1086b91c
108a7b04  040050e1  cmp r0, r4
108a7b08  0800001a  bne 0x108a7b30
108a7b0c  1f00a0e3  mov r0, #0x1f
108a7b10  810fffeb  bl 0x1086b91c
108a7b14  00008de5  str r0, [sp,#0x0]
108a7b18  0110a0e3  mov r1, #0x1
108a7b1c  0d00a0e1  cpy r0, sp
108a7b20  603cffeb  bl 0x10876ca8
108a7b24  00109de5  ldr r1, [sp,#0x0]
108a7b28  1f00a0e3  mov r0, #0x1f
108a7b2c  68f7feeb  bl 0x108658d4
108a7b30  3880bde8  ldmia sp!,{r3,r4,r5,pc}
