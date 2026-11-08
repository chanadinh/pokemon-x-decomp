108abd10  f8402de9  stmdb sp!,{r3,r4,r5,r6,r7,lr}
108abd14  0140a0e1  cpy r4, r1
108abd18  0060a0e1  cpy r6, r0
108abd1c  000093e5  ldr r0, [r3,#0x0]
108abd20  0250a0e1  cpy r5, r2
108abd24  000050e3  cmp r0, #0x0
108abd28  0d00001a  bne 0x108abd64
108abd2c  0210a0e1  cpy r1, r2
108abd30  0400a0e1  cpy r0, r4
108abd34  d9a1feeb  bl 0x108544a0
108abd38  0f10a0e3  mov r1, #0xf
108abd3c  51abfeeb  bl 0x10856a88
108abd40  000050e3  cmp r0, #0x0
108abd44  0600001a  bne 0x108abd64
108abd48  0000a0e3  mov r0, #0x0
108abd4c  00008de5  str r0, [sp,#0x0]
108abd50  0230a0e3  mov r3, #0x2
108abd54  0520a0e1  cpy r2, r5
108abd58  0410a0e1  cpy r1, r4
108abd5c  0600a0e1  cpy r0, r6
108abd60  0c81feeb  bl 0x1084c198
108abd64  f880bde8  ldmia sp!,{r3,r4,r5,r6,r7,pc}
