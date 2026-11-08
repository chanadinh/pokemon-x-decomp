108c36c8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c36cc  0150a0e1  cpy r5, r1
108c36d0  0240a0e1  cpy r4, r2
108c36d4  0200a0e3  mov r0, #0x2
108c36d8  8fa0feeb  bl 0x1086b91c
108c36dc  040050e1  cmp r0, r4
108c36e0  0e00001a  bne 0x108c3720
108c36e4  0410a0e1  cpy r1, r4
108c36e8  0500a0e1  cpy r0, r5
108c36ec  6b43feeb  bl 0x108544a0
108c36f0  b201d0e1  ldrh r0, [r0,#0x12]
108c36f4  000050e3  cmp r0, #0x0
108c36f8  0800000a  beq 0x108c3720
108c36fc  0420a0e1  cpy r2, r4
108c3700  2210a0e3  mov r1, #0x22
108c3704  0500a0e1  cpy r0, r5
108c3708  1b9afeeb  bl 0x10869f7c
108c370c  0010a0e1  cpy r1, r0
108c3710  0440c0e5  strb r4, [r0,#0x4]
108c3714  0500a0e1  cpy r0, r5
108c3718  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c371c  0991feea  b 0x10867b48
108c3720  7080bde8  ldmia sp!,{r4,r5,r6,pc}
