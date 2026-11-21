1089e5f8  900100e0  mul r0, r0, r1
1089e5fc  001aa0e1  mov r1, r0, lsl #0x14
1089e600  2006a0e1  mov r0, r0, lsr #0xc
1089e604  211aa0e1  mov r1, r1, lsr #0x14
1089e608  020b51e3  cmp r1, #0x800
1089e60c  01008082  addhi r0, r0, #0x1
1089e610  000050e3  cmp r0, #0x0
1089e614  0100a003  moveq r0, #0x1
1089e618  1eff2fe1  bx lr
