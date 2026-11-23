1086fd18  ff1001e2  and r1, r1, #0xff
1086fd1c  011481e1  orr r1, r1, r1, lsl #0x8
1086fd20  011881e1  orr r1, r1, r1, lsl #0x10
1086fd24  001080e5  str r1, [r0,#0x0]
1086fd28  b410c0e1  strh r1, [r0,#0x4]
1086fd2c  1eff2fe1  bx lr
