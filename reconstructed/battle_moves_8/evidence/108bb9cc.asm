108bb9cc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bb9d0  0150a0e1  cpy r5, r1
108bb9d4  0240a0e1  cpy r4, r2
108bb9d8  0300a0e3  mov r0, #0x3
108bb9dc  cebffeeb  bl 0x1086b91c
108bb9e0  040050e1  cmp r0, r4
108bb9e4  1700001a  bne 0x108bba48
108bb9e8  0400a0e3  mov r0, #0x4
108bb9ec  cabffeeb  bl 0x1086b91c
108bb9f0  ff1000e2  and r1, r0, #0xff
108bb9f4  0500a0e1  cpy r0, r5
108bb9f8  a862feeb  bl 0x108544a0
108bb9fc  0060a0e1  cpy r6, r0
108bba00  0410a0e1  cpy r1, r4
108bba04  0500a0e1  cpy r0, r5
108bba08  a462feeb  bl 0x108544a0
108bba0c  0040a0e1  cpy r4, r0
108bba10  0d10a0e3  mov r1, #0xd
108bba14  0600a0e1  cpy r0, r6
108bba18  80e2feeb  bl 0x10874420
108bba1c  0050a0e1  cpy r5, r0
108bba20  0d10a0e3  mov r1, #0xd
108bba24  0400a0e1  cpy r0, r4
108bba28  7ce2feeb  bl 0x10874420
108bba2c  000045e0  sub r0, r5, r0
108bba30  000050e3  cmp r0, #0x0
108bba34  030000ca  bgt 0x108bba48
108bba38  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bba3c  0110a0e3  mov r1, #0x1
108bba40  4100a0e3  mov r0, #0x41
108bba44  a2a7feea  b 0x108658d4
108bba48  7080bde8  ldmia sp!,{r4,r5,r6,pc}
