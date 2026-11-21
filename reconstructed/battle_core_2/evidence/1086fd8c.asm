1086fd8c  060051e3  cmp r1, #0x6
1086fd90  0020a0e3  mov r2, #0x0
1086fd94  0110a033  movcc r1, #0x1
1086fd98  0b00002a  bcs 0x1086fdcc
1086fd9c  013180e0  add r3, r0, r1, lsl #0x2
1086fda0  1c2083e5  str r2, [r3,#0x1c]
1086fda4  013080e0  add r3, r0, r1
1086fda8  b420c3e5  strb r2, [r3,#0xb4]
1086fdac  7130ffe6  uxth r3, r1
1086fdb0  020053e3  cmp r3, #0x2
1086fdb4  40208005  streq r2, [r0,#0x40]
1086fdb8  011081e2  add r1, r1, #0x1
1086fdbc  bd20c005  strbeq r2, [r0,#0xbd]
1086fdc0  060051e3  cmp r1, #0x6
1086fdc4  f4ffff3a  bcc 0x1086fd9c
1086fdc8  1eff2fe1  bx lr
1086fdcc  013180e0  add r3, r0, r1, lsl #0x2
1086fdd0  010080e0  add r0, r0, r1
1086fdd4  1c2083e5  str r2, [r3,#0x1c]
1086fdd8  b420c0e5  strb r2, [r0,#0xb4]
1086fddc  1eff2fe1  bx lr
