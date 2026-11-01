1084d29c  b220d0e1  ldrh r2, [r0,#0x2]
1084d2a0  0830a0e3  mov r3, #0x8
1084d2a4  02c9a0e1  mov r12, r2, lsl #0x12
1084d2a8  ac0c53e1  cmp r3, r12, lsr #0x19
1084d2ac  0700009a  bls 0x1084d2d0
1084d2b0  02c4a0e3  mov r12, #0x2000000
1084d2b4  7f3fa0e3  mov r3, #0x1fc
1084d2b8  62c78ce0  add r12, r12, r2, ror #0xe
1084d2bc  a22203e0  and r2, r3, r2, lsr #0x5
1084d2c0  6c39a0e1  mov r3, r12, ror #0x12
1084d2c4  002082e0  add r2, r2, r0
1084d2c8  b230c0e1  strh r3, [r0,#0x2]
1084d2cc  041082e5  str r1, [r2,#0x4]
1084d2d0  1eff2fe1  bx lr
