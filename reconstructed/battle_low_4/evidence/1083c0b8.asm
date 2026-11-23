1083c0b8  08219fe5  ldr r2, [0x1083c1c8]
1083c0bc  090051e3  cmp r1, #0x9
1083c0c0  0030a0e3  mov r3, #0x0
1083c0c4  01f19f37  ldrcc pc, [pc,r1,lsl #0x2]
1083c0c8  1eff2fe1  bx lr
1083c0f0  9c3092e5  ldr r3, [r2,#0x9c]
1083c0f4  a02092e5  ldr r2, [r2,#0xa0]
1083c0f8  0110a0e3  mov r1, #0x1
1083c0fc  342780e5  str r2, [r0,#0x734]
1083c100  303780e5  str r3, [r0,#0x730]
1083c104  2c17c0e5  strb r1, [r0,#0x72c]
1083c108  1eff2fe1  bx lr
1083c10c  a42082e2  add r2, r2, #0xa4
1083c110  73ce80e2  add r12, r0, #0x730
1083c114  060092e8  ldmia r2,{r1,r2}
1083c118  06008ce8  stmia r12,{r1,r2}
1083c11c  2c37c0e5  strb r3, [r0,#0x72c]
1083c120  1eff2fe1  bx lr
1083c124  ac2082e2  add r2, r2, #0xac
1083c128  73ce80e2  add r12, r0, #0x730
1083c12c  060092e8  ldmia r2,{r1,r2}
1083c130  06008ce8  stmia r12,{r1,r2}
1083c134  2c37c0e5  strb r3, [r0,#0x72c]
1083c138  1eff2fe1  bx lr
1083c13c  b42082e2  add r2, r2, #0xb4
1083c140  73ce80e2  add r12, r0, #0x730
1083c144  060092e8  ldmia r2,{r1,r2}
1083c148  06008ce8  stmia r12,{r1,r2}
1083c14c  2c37c0e5  strb r3, [r0,#0x72c]
1083c150  1eff2fe1  bx lr
1083c154  bc2082e2  add r2, r2, #0xbc
1083c158  73ce80e2  add r12, r0, #0x730
1083c15c  060092e8  ldmia r2,{r1,r2}
1083c160  06008ce8  stmia r12,{r1,r2}
1083c164  2c37c0e5  strb r3, [r0,#0x72c]
1083c168  1eff2fe1  bx lr
1083c16c  c42082e2  add r2, r2, #0xc4
1083c170  73ce80e2  add r12, r0, #0x730
1083c174  060092e8  ldmia r2,{r1,r2}
1083c178  06008ce8  stmia r12,{r1,r2}
1083c17c  2c37c0e5  strb r3, [r0,#0x72c]
1083c180  1eff2fe1  bx lr
1083c184  cc2082e2  add r2, r2, #0xcc
1083c188  73ce80e2  add r12, r0, #0x730
1083c18c  060092e8  ldmia r2,{r1,r2}
1083c190  06008ce8  stmia r12,{r1,r2}
1083c194  2c37c0e5  strb r3, [r0,#0x72c]
1083c198  1eff2fe1  bx lr
1083c19c  d42082e2  add r2, r2, #0xd4
1083c1a0  73ce80e2  add r12, r0, #0x730
1083c1a4  060092e8  ldmia r2,{r1,r2}
1083c1a8  06008ce8  stmia r12,{r1,r2}
1083c1ac  2c37c0e5  strb r3, [r0,#0x72c]
1083c1b0  1eff2fe1  bx lr
1083c1b4  2c18d0e5  ldrb r1, [r0,#0x82c]
1083c1b8  000051e3  cmp r1, #0x0
1083c1bc  01005113  cmpne r1, #0x1
1083c1c0  d1ffff0a  beq 0x1083c10c
1083c1c4  1eff2fe1  bx lr
