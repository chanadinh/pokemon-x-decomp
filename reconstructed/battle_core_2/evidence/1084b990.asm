1084b990  002090e5  ldr r2, [r0,#0x0]
1084b994  4cc09fe5  ldr r12, [0x1084b9e8]
1084b998  073002e2  and r3, r2, #0x7
1084b99c  010053e3  cmp r3, #0x1
1084b9a0  03005313  cmpne r3, #0x3
1084b9a4  0700000a  beq 0x1084b9c8
1084b9a8  040053e3  cmp r3, #0x4
1084b9ac  0900000a  beq 0x1084b9d8
1084b9b0  020053e3  cmp r3, #0x2
1084b9b4  0c20c201  biceq r2, r2, r12
1084b9b8  81148201  orreq r1, r2, r1, lsl #0x9
1084b9bc  0000001a  bne 0x1084b9c4
1084b9c0  001080e5  str r1, [r0,#0x0]
1084b9c4  1eff2fe1  bx lr
1084b9c8  0c20c2e1  bic r2, r2, r12
1084b9cc  811482e1  orr r1, r2, r1, lsl #0x9
1084b9d0  001080e5  str r1, [r0,#0x0]
1084b9d4  1eff2fe1  bx lr
1084b9d8  0c309fe5  ldr r3, [0x1084b9ec]
1084b9dc  032002e0  and r2, r2, r3
1084b9e0  811782e1  orr r1, r2, r1, lsl #0xf
1084b9e4  f5ffffea  b 0x1084b9c0
