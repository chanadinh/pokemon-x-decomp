108c0578  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c057c  0160a0e1  cpy r6, r1
108c0580  0250a0e1  cpy r5, r2
108c0584  0300a0e3  mov r0, #0x3
108c0588  e3acfeeb  bl 0x1086b91c
108c058c  050050e1  cmp r0, r5
108c0590  1400001a  bne 0x108c05e8
108c0594  1600a0e3  mov r0, #0x16
108c0598  dfacfeeb  bl 0x1086b91c
108c059c  ff4000e2  and r4, r0, #0xff
108c05a0  120054e3  cmp r4, #0x12
108c05a4  0f00002a  bcs 0x108c05e8
108c05a8  0520a0e1  cpy r2, r5
108c05ac  1310a0e3  mov r1, #0x13
108c05b0  0600a0e1  cpy r0, r6
108c05b4  70a6feeb  bl 0x10869f7c
108c05b8  0010a0e1  cpy r1, r0
108c05bc  000090e5  ldr r0, [r0,#0x0]
108c05c0  1f2004e2  and r2, r4, #0x1f
108c05c4  0650c1e5  strb r5, [r1,#0x6]
108c05c8  020580e3  orr r0, r0, #0x800000
108c05cc  822282e1  orr r2, r2, r2, lsl #0x5
108c05d0  000081e5  str r0, [r1,#0x0]
108c05d4  122b82e3  orr r2, r2, #0x4800
108c05d8  0600a0e1  cpy r0, r6
108c05dc  b420c1e1  strh r2, [r1,#0x4]
108c05e0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c05e4  579dfeea  b 0x10867b48
108c05e8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
