1086b284  0010a0e1  cpy r1, r0
1086b288  000090e5  ldr r0, [r0,#0x0]
1086b28c  2025a0e1  mov r2, r0, lsr #0xa
1086b290  0225a0e1  mov r2, r2, lsl #0xa
1086b294  8236a0e1  mov r3, r2, lsl #0xd
1086b298  0323c2e3  bic r2, r2, #0xc000000
1086b29c  a33ba0e1  mov r3, r3, lsr #0x17
1086b2a0  fe27c2e3  bic r2, r2, #0x3f80000
1086b2a4  832982e1  orr r2, r2, r3, lsl #0x13
1086b2a8  0722c2e3  bic r2, r2, #0x70000000
1086b2ac  002081e5  str r2, [r1,#0x0]
1086b2b0  1eff2fe1  bx lr
