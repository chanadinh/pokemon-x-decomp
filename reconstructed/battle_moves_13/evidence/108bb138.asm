108bb138  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108bb13c  0160a0e1  cpy r6, r1
108bb140  0250a0e1  cpy r5, r2
108bb144  0200a0e1  cpy r0, r2
108bb148  9c43feeb  bl 0x1084bfc0
108bb14c  6b3ffeeb  bl 0x1084af00
108bb150  0040a0e1  cpy r4, r0
108bb154  0520a0e1  cpy r2, r5
108bb158  1b10a0e3  mov r1, #0x1b
108bb15c  0600a0e1  cpy r0, r6
108bb160  85bbfeeb  bl 0x10869f7c
108bb164  0050a0e1  cpy r5, r0
108bb168  0410a0e3  mov r1, #0x4
108bb16c  0840c0e5  strb r4, [r0,#0x8]
108bb170  0410e0e5  strb r1, [r0,#0x4]!
108bb174  0010a0e3  mov r1, #0x0
108bb178  0120a0e3  mov r2, #0x1
108bb17c  0110e0e5  strb r1, [r0,#0x1]!
108bb180  012052e2  subs r2, r2, #0x1
108bb184  0110c0e5  strb r1, [r0,#0x1]
108bb188  0210e0e5  strb r1, [r0,#0x2]!
108bb18c  fbffff1a  bne 0x108bb180
108bb190  0040a0e3  mov r4, #0x0
108bb194  0170a0e3  mov r7, #0x1
108bb198  ff0004e2  and r0, r4, #0xff
108bb19c  9140feeb  bl 0x1084b3e8
108bb1a0  000050e3  cmp r0, #0x0
108bb1a4  00f020e3  nop
108bb1a8  0900000a  beq 0x108bb1d4
108bb1ac  0430d5e5  ldrb r3, [r5,#0x4]
108bb1b0  a41187e0  add r1, r7, r4, lsr #0x3
108bb1b4  ff1001e2  and r1, r1, #0xff
108bb1b8  010053e1  cmp r3, r1
108bb1bc  040085e2  add r0, r5, #0x4
108bb1c0  072004e2  and r2, r4, #0x7
108bb1c4  0200009a  bls 0x108bb1d4
108bb1c8  0130d0e7  ldrb r3, [r0,r1]
108bb1cc  172283e1  orr r2, r3, r7, lsl r2
108bb1d0  0120c0e7  strb r2, [r0,r1]
108bb1d4  014084e2  add r4, r4, #0x1
108bb1d8  110054e3  cmp r4, #0x11
108bb1dc  edffff3a  bcc 0x108bb198
108bb1e0  0510a0e1  cpy r1, r5
108bb1e4  0600a0e1  cpy r0, r6
108bb1e8  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108bb1ec  55b2feea  b 0x10867b48
