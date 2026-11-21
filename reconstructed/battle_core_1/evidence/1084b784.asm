1084b784  74309fe5  ldr r3, [0x1084b800]
1084b788  30002de9  stmdb sp!,{r4,r5}
1084b78c  00c0d3e7  ldrb r12, [r3,r0]
1084b790  0030a0e3  mov r3, #0x0
1084b794  00005ce3  cmp r12, #0x0
1084b798  1500009a  bls 0x1084b7f4
1084b79c  034280e0  add r4, r0, r3, lsl #0x4
1084b7a0  014a84e2  add r4, r4, #0x1000
1084b7a4  bc5094e5  ldr r5, [r4,#0xbc]
1084b7a8  1950d5e5  ldrb r5, [r5,#0x19]
1084b7ac  010055e1  cmp r5, r1
1084b7b0  0c00001a  bne 0x1084b7e8
1084b7b4  c04094e5  ldr r4, [r4,#0xc0]
1084b7b8  0f4004e2  and r4, r4, #0xf
1084b7bc  060054e3  cmp r4, #0x6
1084b7c0  0800000a  beq 0x1084b7e8
1084b7c4  080054e3  cmp r4, #0x8
1084b7c8  0600000a  beq 0x1084b7e8
1084b7cc  030280e0  add r0, r0, r3, lsl #0x4
1084b7d0  010a80e2  add r0, r0, #0x1000
1084b7d4  c00090e5  ldr r0, [r0,#0xc0]
1084b7d8  000082e5  str r0, [r2,#0x0]
1084b7dc  3000bde8  ldmia sp!,{r4,r5}
1084b7e0  0100a0e3  mov r0, #0x1
1084b7e4  1eff2fe1  bx lr
1084b7e8  013083e2  add r3, r3, #0x1
1084b7ec  03005ce1  cmp r12, r3
1084b7f0  e9ffff8a  bhi 0x1084b79c
1084b7f4  3000bde8  ldmia sp!,{r4,r5}
1084b7f8  0000a0e3  mov r0, #0x0
1084b7fc  1eff2fe1  bx lr
