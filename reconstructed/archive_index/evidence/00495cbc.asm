00495cbc  7130ffe6  uxth r3,r1
00495cc0  30002de9  stmdb sp!,{r4,r5}
00495cc4  081090e5  ldr r1,[r0,#0x8]
00495cc8  0c0090e5  ldr r0,[r0,#0xc]
00495ccc  01c0a0e3  mov r12,#0x1
00495cd0  0c1081e2  add r1,r1,#0xc
00495cd4  031191e7  ldr r1,[r1,r3,lsl #0x2]
00495cd8  0c3081e2  add r3,r1,#0xc
00495cdc  101081e2  add r1,r1,#0x10
00495ce0  034090e7  ldr r4,[r0,r3]
00495ce4  015080e0  add r5,r0,r1
00495ce8  1c0214e1  tst r4,r12, lsl r2
00495cec  0010a0e3  mov r1,#0x0
00495cf0  0900000a  beq 0x00495d1c
00495cf4  000052e3  cmp r2,#0x0
00495cf8  0000a0e3  mov r0,#0x0
00495cfc  0600009a  bls 0x00495d1c
00495d00  1c0014e1  tst r4,r12, lsl r0
00495d04  01108112  addne r1,r1,#0x1
00495d08  010080e2  add r0,r0,#0x1
00495d0c  7110ff16  uxthne r1,r1
00495d10  7000ffe6  uxth r0,r0
00495d14  020050e1  cmp r0,r2
00495d18  f8ffff3a  bcc 0x00495d00
00495d1c  810081e0  add r0,r1,r1, lsl #0x1
00495d20  000185e0  add r0,r5,r0, lsl #0x2
00495d24  080090e5  ldr r0,[r0,#0x8]
00495d28  3000bde8  ldmia sp!,{r4,r5}
00495d2c  1eff2fe1  bx lr
