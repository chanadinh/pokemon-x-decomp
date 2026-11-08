108c31ac  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c31b0  0160a0e1  cpy r6, r1
108c31b4  0240a0e1  cpy r4, r2
108c31b8  0350a0e1  cpy r5, r3
108c31bc  0200a0e3  mov r0, #0x2
108c31c0  d5a1feeb  bl 0x1086b91c
108c31c4  040050e1  cmp r0, r4
108c31c8  00009505  ldreq r0, [r5,#0x0]
108c31cc  01005003  cmpeq r0, #0x1
108c31d0  0900001a  bne 0x108c31fc
108c31d4  0410a0e1  cpy r1, r4
108c31d8  0600a0e1  cpy r0, r6
108c31dc  af44feeb  bl 0x108544a0
108c31e0  b201d0e1  ldrh r0, [r0,#0x12]
108c31e4  000050e3  cmp r0, #0x0
108c31e8  0300001a  bne 0x108c31fc
108c31ec  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c31f0  021aa0e3  mov r1, #0x2000
108c31f4  3600a0e3  mov r0, #0x36
108c31f8  2127feea  b 0x1084ce84
108c31fc  7080bde8  ldmia sp!,{r4,r5,r6,pc}
