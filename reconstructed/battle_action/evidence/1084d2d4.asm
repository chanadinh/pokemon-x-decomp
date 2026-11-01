1084d2d4  b230d0e1  ldrh r3, [r0,#0x2]
1084d2d8  b020c0e1  strh r2, [r0,#0x0]
1084d2dc  7f1001e2  and r1, r1, #0x7f
1084d2e0  7f20c3e3  bic r2, r3, #0x7f
1084d2e4  021081e1  orr r1, r1, r2
1084d2e8  fe1dc1e3  bic r1, r1, #0x3f80
1084d2ec  b210c0e1  strh r1, [r0,#0x2]
1084d2f0  1eff2fe1  bx lr
