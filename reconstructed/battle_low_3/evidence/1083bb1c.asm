1083bb1c  f04f2de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11,lr}
1083bb20  0060a0e1  cpy r6, r0
1083bb24  0270a0e1  cpy r7, r2
1083bb28  0cd04de2  sub sp, sp, #0xc
1083bb2c  0380a0e1  cpy r8, r3
1083bb30  d8209fe5  ldr r2, [0x1083bc10]
1083bb34  d8009fe5  ldr r0, [0x1083bc14]
1083bb38  30309de5  ldr r3, [sp,#0x30]
1083bb3c  6cc092e5  ldr r12, [r2,#0x6c]
1083bb40  00005ce1  cmp r12, r0
1083bb44  6c008215  strne r0, [r2,#0x6c]
1083bb48  760e86e2  add r0, r6, #0x760
1083bb4c  0a0080e8  stmia r0,{r1,r3}
1083bb50  000096e5  ldr r0, [r6,#0x0]
1083bb54  241090e5  ldr r1, [r0,#0x24]
1083bb58  0600a0e1  cpy r0, r6
1083bb5c  31ff2fe1  blx r1
1083bb60  0090a0e1  cpy r9, r0
1083bb64  1800d0e5  ldrb r0, [r0,#0x18]
1083bb68  0040a0e3  mov r4, #0x0
1083bb6c  005050e2  subs r5, r0, #0x0
1083bb70  0db0a0e1  cpy r11, sp
1083bb74  04a0a0e1  cpy r10, r4
1083bb78  0c00009a  bls 0x1083bbb0
1083bb7c  ff1004e2  and r1, r4, #0xff
1083bb80  0900a0e1  cpy r0, r9
1083bb84  db8700eb  bl 0x1085daf8
1083bb88  1310a0e3  mov r1, #0x13
1083bb8c  00f020e3  nop
1083bb90  bc6b00eb  bl 0x10856a88
1083bb94  0400cbe7  strb r0, [r11,r4]
1083bb98  014084e2  add r4, r4, #0x1
1083bb9c  050054e1  cmp r4, r5
1083bba0  f5ffff3a  bcc 0x1083bb7c
1083bba4  060064e2  rsb r0, r4, #0x6
1083bba8  000050e3  cmp r0, #0x0
1083bbac  0a0000da  ble 0x1083bbdc
1083bbb0  04008be0  add r0, r11, r4
1083bbb4  061064e2  rsb r1, r4, #0x6
1083bbb8  010040e2  sub r0, r0, #0x1
1083bbbc  010011e3  tst r1, #0x1
1083bbc0  01a0e015  strbne r10, [r0,#0x1]!
1083bbc4  c110b0e1  movs r1, r1, asr #0x1
1083bbc8  0300000a  beq 0x1083bbdc
1083bbcc  01a0c0e5  strb r10, [r0,#0x1]
1083bbd0  011051e2  subs r1, r1, #0x1
1083bbd4  02a0e0e5  strb r10, [r0,#0x2]!
1083bbd8  fbffff1a  bne 0x1083bbcc
1083bbdc  900196e5  ldr r0, [r6,#0x190]
1083bbe0  012028e2  eor r2, r8, #0x1
1083bbe4  0d10a0e1  cpy r1, sp
1083bbe8  3ca080e5  str r10, [r0,#0x3c]
1083bbec  4a20c0e5  strb r2, [r0,#0x4a]
1083bbf0  3821c0e5  strb r2, [r0,#0x138]
1083bbf4  3f71c0e5  strb r7, [r0,#0x13f]
1083bbf8  4070c0e5  strb r7, [r0,#0x40]
1083bbfc  010c80e2  add r0, r0, #0x100
1083bc00  390080e2  add r0, r0, #0x39
1083bc04  a08f00eb  bl 0x1085fa8c
1083bc08  0cd08de2  add sp, sp, #0xc
1083bc0c  f08fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,pc}
