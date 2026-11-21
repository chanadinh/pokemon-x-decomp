1084b3f8  70402de9  stmdb sp!,{r4,r5,r6,lr}
1084b3fc  0050a0e1  cpy r5, r0
1084b400  0160a0e1  cpy r6, r1
1084b404  010a80e2  add r0, r0, #0x1000
1084b408  0240a0e1  cpy r4, r2
1084b40c  0210a0e1  cpy r1, r2
1084b410  830f80e2  add r0, r0, #0x20c
1084b414  ac8300eb  bl 0x1086c2cc
1084b418  c03d95e5  ldr r3, [r5,#0xdc0]
1084b41c  0010d3e5  ldrb r1, [r3,#0x0]
1084b420  0a0051e3  cmp r1, #0xa
1084b424  0a00002a  bcs 0x1084b454
1084b428  01c083e2  add r12, r3, #0x1
1084b42c  152083e2  add r2, r3, #0x15
1084b430  0c60c1e7  strb r6, [r1,r12]
1084b434  00c0d3e5  ldrb r12, [r3,#0x0]
1084b438  0b1083e2  add r1, r3, #0xb
1084b43c  0240cce7  strb r4, [r12,r2]
1084b440  0020d3e5  ldrb r2, [r3,#0x0]
1084b444  0100c2e7  strb r0, [r2,r1]
1084b448  0000d3e5  ldrb r0, [r3,#0x0]
1084b44c  010080e2  add r0, r0, #0x1
1084b450  0000c3e5  strb r0, [r3,#0x0]
1084b454  7080bde8  ldmia sp!,{r4,r5,r6,pc}
