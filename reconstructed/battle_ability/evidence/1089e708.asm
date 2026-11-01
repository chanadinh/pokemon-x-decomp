1089e708  900100e0  mul r0, r0, r1
1089e70c  001aa0e1  mov r1, r0, lsl #0x14
1089e710  2006a0e1  mov r0, r0, lsr #0xc
1089e714  211aa0e1  mov r1, r1, lsr #0x14
1089e718  020b51e3  cmp r1, #0x800
1089e71c  01008082  addhi r0, r0, #0x1
1089e720  1eff2fe1  bx lr
