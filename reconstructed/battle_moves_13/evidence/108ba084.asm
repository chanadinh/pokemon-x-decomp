108ba084  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108ba088  0160a0e1  cpy r6, r1
108ba08c  0250a0e1  cpy r5, r2
108ba090  0200a0e1  cpy r0, r2
108ba094  c947feeb  bl 0x1084bfc0
108ba098  9843feeb  bl 0x1084af00
108ba09c  0040a0e1  cpy r4, r0
108ba0a0  0520a0e1  cpy r2, r5
108ba0a4  1b10a0e3  mov r1, #0x1b
108ba0a8  0600a0e1  cpy r0, r6
108ba0ac  b2bffeeb  bl 0x10869f7c
108ba0b0  0170a0e3  mov r7, #0x1
108ba0b4  0840c0e5  strb r4, [r0,#0x8]
108ba0b8  0050a0e1  cpy r5, r0
108ba0bc  0410a0e3  mov r1, #0x4
108ba0c0  0970c0e5  strb r7, [r0,#0x9]
108ba0c4  0410e0e5  strb r1, [r0,#0x4]!
108ba0c8  0010a0e3  mov r1, #0x0
108ba0cc  0720a0e1  cpy r2, r7
108ba0d0  0110e0e5  strb r1, [r0,#0x1]!
108ba0d4  012052e2  subs r2, r2, #0x1
108ba0d8  0110c0e5  strb r1, [r0,#0x1]
108ba0dc  0210e0e5  strb r1, [r0,#0x2]!
108ba0e0  fbffff1a  bne 0x108ba0d4
108ba0e4  0040a0e3  mov r4, #0x0
108ba0e8  ff0004e2  and r0, r4, #0xff
108ba0ec  bd44feeb  bl 0x1084b3e8
108ba0f0  000050e3  cmp r0, #0x0
108ba0f4  00f020e3  nop
108ba0f8  0900000a  beq 0x108ba124
108ba0fc  0430d5e5  ldrb r3, [r5,#0x4]
108ba100  a41187e0  add r1, r7, r4, lsr #0x3
108ba104  ff1001e2  and r1, r1, #0xff
108ba108  010053e1  cmp r3, r1
108ba10c  040085e2  add r0, r5, #0x4
108ba110  072004e2  and r2, r4, #0x7
108ba114  0200009a  bls 0x108ba124
108ba118  0130d0e7  ldrb r3, [r0,r1]
108ba11c  172283e1  orr r2, r3, r7, lsl r2
108ba120  0120c0e7  strb r2, [r0,r1]
108ba124  014084e2  add r4, r4, #0x1
108ba128  110054e3  cmp r4, #0x11
108ba12c  edffff3a  bcc 0x108ba0e8
108ba130  0510a0e1  cpy r1, r5
108ba134  0600a0e1  cpy r0, r6
108ba138  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108ba13c  81b6feea  b 0x10867b48
