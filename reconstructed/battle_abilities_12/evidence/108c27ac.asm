108c27ac  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c27b0  0150a0e1  cpy r5, r1
108c27b4  0240a0e1  cpy r4, r2
108c27b8  2300a0e3  mov r0, #0x23
108c27bc  56a4feeb  bl 0x1086b91c
108c27c0  060050e3  cmp r0, #0x6
108c27c4  1700001a  bne 0x108c2828
108c27c8  0200a0e3  mov r0, #0x2
108c27cc  52a4feeb  bl 0x1086b91c
108c27d0  040050e1  cmp r0, r4
108c27d4  1300001a  bne 0x108c2828
108c27d8  0420a0e1  cpy r2, r4
108c27dc  0d10a0e3  mov r1, #0xd
108c27e0  0500a0e1  cpy r0, r5
108c27e4  e49dfeeb  bl 0x10869f7c
108c27e8  0010a0e1  cpy r1, r0
108c27ec  000090e5  ldr r0, [r0,#0x0]
108c27f0  18c0d1e5  ldrb r12, [r1,#0x18]
108c27f4  0530a0e3  mov r3, #0x5
108c27f8  0430c1e5  strb r3, [r1,#0x4]
108c27fc  020580e3  orr r0, r0, #0x800000
108c2800  000081e5  str r0, [r1,#0x0]
108c2804  0120a0e3  mov r2, #0x1
108c2808  0230cce3  bic r3, r12, #0x2
108c280c  1720c1e5  strb r2, [r1,#0x17]
108c2810  1830c1e5  strb r3, [r1,#0x18]
108c2814  1620c1e5  strb r2, [r1,#0x16]
108c2818  0500a0e1  cpy r0, r5
108c281c  0c40c1e5  strb r4, [r1,#0xc]
108c2820  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c2824  c794feea  b 0x10867b48
108c2828  7080bde8  ldmia sp!,{r4,r5,r6,pc}
