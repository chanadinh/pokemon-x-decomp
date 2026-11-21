1089e5ac  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
1089e5b0  0050a0e3  mov r5, #0x0
1089e5b4  0070a0e1  cpy r7, r0
1089e5b8  0160a0e1  cpy r6, r1
1089e5bc  0540a0e1  cpy r4, r5
1089e5c0  ff1004e2  and r1, r4, #0xff
1089e5c4  0700a0e1  cpy r0, r7
1089e5c8  ac09ffeb  bl 0x10860c80
1089e5cc  060050e3  cmp r0, #0x6
1089e5d0  00005013  cmpne r0, #0x0
1089e5d4  0200001a  bne 0x1089e5e4
1089e5d8  010085e2  add r0, r5, #0x1
1089e5dc  0540c6e7  strb r4, [r6,r5]
1089e5e0  ff5000e2  and r5, r0, #0xff
1089e5e4  014084e2  add r4, r4, #0x1
1089e5e8  120054e3  cmp r4, #0x12
1089e5ec  f3ffff3a  bcc 0x1089e5c0
1089e5f0  0500a0e1  cpy r0, r5
1089e5f4  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
