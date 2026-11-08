108b4aec  f8402de9  stmdb sp!,{r3,r4,r5,r6,r7,lr}
108b4af0  0150a0e1  cpy r5, r1
108b4af4  0240a0e1  cpy r4, r2
108b4af8  0200a0e3  mov r0, #0x2
108b4afc  86dbfeeb  bl 0x1086b91c
108b4b00  040050e1  cmp r0, r4
108b4b04  1000001a  bne 0x108b4b4c
108b4b08  0d00a0e1  cpy r0, sp
108b4b0c  63bfffeb  bl 0x108a48a0
108b4b10  00109de5  ldr r1, [sp,#0x0]
108b4b14  7110ffe6  uxth r1, r1
108b4b18  01a7ffeb  bl 0x1089e724
108b4b1c  0060a0e1  cpy r6, r0
108b4b20  0020a0e1  cpy r2, r0
108b4b24  0410a0e1  cpy r1, r4
108b4b28  0500a0e1  cpy r0, r5
108b4b2c  1ef4feeb  bl 0x10871bac
108b4b30  0040a0e1  cpy r4, r0
108b4b34  0610a0e1  cpy r1, r6
108b4b38  1200a0e3  mov r0, #0x12
108b4b3c  64c3feeb  bl 0x108658d4
108b4b40  0410a0e1  cpy r1, r4
108b4b44  0d00a0e3  mov r0, #0xd
108b4b48  61c3feeb  bl 0x108658d4
108b4b4c  f880bde8  ldmia sp!,{r3,r4,r5,r6,r7,pc}
