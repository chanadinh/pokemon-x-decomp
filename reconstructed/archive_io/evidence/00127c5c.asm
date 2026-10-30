00127c5c  f8432de9  stmdb sp!,{r3,r4,r5,r6,r7,r8,r9,lr}
00127c60  0040a0e1  cpy r4, r0
00127c64  0160a0e1  cpy r6, r1
00127c68  0270a0e1  cpy r7, r2
00127c6c  20509de5  ldr r5, [sp,#0x20]
00127c70  0380a0e1  cpy r8, r3
00127c74  600080e2  add r0, r0, #0x60
00127c78  eade00eb  bl 0x0015f828
00127c7c  0010a0e3  mov r1, #0x0
00127c80  0620a0e1  cpy r2, r6
00127c84  c63fa0e1  mov r3, r6, asr #0x1f
00127c88  180084e2  add r0, r4, #0x18
00127c8c  00108de5  str r1, [sp,#0x0]
00127c90  892e00eb  bl 0x001336bc
00127c94  021110e2  ands r1, r0, #0x80000000
00127c98  0200005a  bpl 0x00127ca8
00127c9c  0f10a0e1  cpy r1, pc
00127ca0  a02fb0e1  movs r2, r0, lsr #0x1f
00127ca4  16f2ff1b  blne 0x00124504
00127ca8  0820a0e1  cpy r2, r8
00127cac  0710a0e1  cpy r1, r7
00127cb0  0400a0e1  cpy r0, r4
00127cb4  acffffeb  bl 0x00127b6c
00127cb8  000055e3  cmp r5, #0x0
00127cbc  00008515  strne r0, [r5,#0x0]
00127cc0  600084e2  add r0, r4, #0x60
00127cc4  f843bde8  ldmia sp!,{r3,r4,r5,r6,r7,r8,r9,lr}
00127cc8  08df00ea  b 0x0015f8f0
