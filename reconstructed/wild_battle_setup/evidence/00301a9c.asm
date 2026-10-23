00301a9c  0020a0e3  mov r2,#0x0
00301aa0  00402de9  stmdb sp!,{lr}
00301aa4  0230a0e1  cpy r3,r2
00301aa8  02c0a0e1  cpy r12,r2
00301aac  02e0a0e1  cpy lr,r2
00301ab0  201051e2  subs r1,r1,#0x20
00301ab4  0c50a028  stmiacs r0!,{r2,r3,r12,lr}
00301ab8  0c50a028  stmiacs r0!,{r2,r3,r12,lr}
00301abc  20105122  subcss r1,r1,#0x20
00301ac0  fbffff2a  bcs 0x00301ab4
00301ac4  011eb0e1  movs r1,r1, lsl #0x1c
00301ac8  0c50a028  stmiacs r0!,{r2,r3,r12,lr}
00301acc  0c00a048  stmiami r0!,{r2,r3}
00301ad0  0040bde8  ldmia sp!,{lr}
00301ad4  0111b0e1  movs r1,r1, lsl #0x2
00301ad8  04208024  strcs r2,[r0],#0x4
00301adc  1eff2f01  bxeq lr
00301ae0  b220c040  strhmi r2,[r0],#0x2
00301ae4  010111e3  tst r1,#0x40000000
00301ae8  0120c014  strbne r2,[r0],#0x1
00301aec  1eff2fe1  bx lr
