0039e7e0  70402de9  stmdb sp!,{r4,r5,r6,lr}
0039e7e4  0060a0e1  cpy r6,r0
0039e7e8  0150a0e1  cpy r5,r1
0039e7ec  0040a0e3  mov r4,#0x0
0039e7f0  041195e7  ldr r1,[r5,r4,lsl #0x2]
0039e7f4  040196e7  ldr r0,[r6,r4,lsl #0x2]
0039e7f8  4befffeb  bl 0x0039a52c
0039e7fc  014084e2  add r4,r4,#0x1
0039e800  060054e3  cmp r4,#0x6
0039e804  f9ffff3a  bcc 0x0039e7f0
0039e808  1800d5e5  ldrb r0,[r5,#0x18]
0039e80c  1800c6e5  strb r0,[r6,#0x18]
0039e810  7080bde8  ldmia sp!,{r4,r5,r6,pc}
