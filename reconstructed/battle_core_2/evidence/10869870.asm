10869870  0020a0e3  mov r2, #0x0
10869874  0a30a0e3  mov r3, #0xa
10869878  0210a0e1  cpy r1, r2
1086987c  04402de5  str r4, [sp,#-0x4]!
10869880  82c080e0  add r12, r0, r2, lsl #0x1
10869884  021180e7  str r1, [r0,r2,lsl #0x2]
10869888  024080e0  add r4, r0, r2
1086988c  b812cce1  strh r1, [r12,#0x28]
10869890  bc13cce1  strh r1, [r12,#0x3c]
10869894  5010c4e5  strb r1, [r4,#0x50]
10869898  013053e2  subs r3, r3, #0x1
1086989c  012082e2  add r2, r2, #0x1
108698a0  ba15cce1  strh r1, [r12,#0x5a]
108698a4  f5ffff1a  bne 0x10869880
108698a8  6e10c0e5  strb r1, [r0,#0x6e]
108698ac  6f10c0e5  strb r1, [r0,#0x6f]
108698b0  7010c0e5  strb r1, [r0,#0x70]
108698b4  7110c0e5  strb r1, [r0,#0x71]
108698b8  04409de4  ldr r4, [sp],#0x4
108698bc  1eff2fe1  bx lr
