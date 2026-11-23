1083b3cc  70402de9  stmdb sp!,{r4,r5,r6,lr}
1083b3d0  0150a0e1  cpy r5, r1
1083b3d4  68109fe5  ldr r1, [0x1083b444]
1083b3d8  0040a0e1  cpy r4, r0
1083b3dc  0260a0e1  cpy r6, r2
1083b3e0  60009fe5  ldr r0, [0x1083b448]
1083b3e4  6c2091e5  ldr r2, [r1,#0x6c]
1083b3e8  08d04de2  sub sp, sp, #0x8
1083b3ec  000052e1  cmp r2, r0
1083b3f0  6c008115  strne r0, [r1,#0x6c]
1083b3f4  000094e5  ldr r0, [r4,#0x0]
1083b3f8  bc1090e5  ldr r1, [r0,#0xbc]
1083b3fc  0400a0e1  cpy r0, r4
1083b400  31ff2fe1  blx r1
1083b404  000050e3  cmp r0, #0x0
1083b408  0b00001a  bne 0x1083b43c
1083b40c  e00194e5  ldr r0, [r4,#0x1e0]
1083b410  0620a0e1  cpy r2, r6
1083b414  0510a0e1  cpy r1, r5
1083b418  8a8801eb  bl 0x1089d648
1083b41c  9001b4e5  ldr r0, [r4,#0x190]!
1083b420  0030a0e3  mov r3, #0x0
1083b424  501094e5  ldr r1, [r4,#0x50]
1083b428  0320a0e1  cpy r2, r3
1083b42c  3a0e80e2  add r0, r0, #0x3a0
1083b430  00308de5  str r3, [sp,#0x0]
1083b434  04308de5  str r3, [sp,#0x4]
1083b438  447300eb  bl 0x10858150
1083b43c  08d08de2  add sp, sp, #0x8
1083b440  7080bde8  ldmia sp!,{r4,r5,r6,pc}
