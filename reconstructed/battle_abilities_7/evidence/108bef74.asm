108bef74  f0402de9  stmdb sp!,{r4,r5,r6,r7,lr}
108bef78  0cd04de2  sub sp, sp, #0xc
108bef7c  0160a0e1  cpy r6, r1
108bef80  0240a0e1  cpy r4, r2
108bef84  0200a0e3  mov r0, #0x2
108bef88  63b2feeb  bl 0x1086b91c
108bef8c  ff0000e2  and r0, r0, #0xff
108bef90  0410a0e1  cpy r1, r4
108bef94  8335feeb  bl 0x1084c5a8
108bef98  000050e3  cmp r0, #0x0
108bef9c  1400001a  bne 0x108beff4
108befa0  0410a0e1  cpy r1, r4
108befa4  0600a0e1  cpy r0, r6
108befa8  f236feeb  bl 0x1084cb78
108befac  011c80e3  orr r1, r0, #0x100
108befb0  0d20a0e1  cpy r2, sp
108befb4  0600a0e1  cpy r0, r6
108befb8  1833feeb  bl 0x1084bc20
108befbc  005050e2  subs r5, r0, #0x0
108befc0  0040a0e3  mov r4, #0x0
108befc4  0d70a081  cpyhi r7, sp
108befc8  0900009a  bls 0x108beff4
108befcc  0410d7e7  ldrb r1, [r7,r4]
108befd0  0600a0e1  cpy r0, r6
108befd4  d233feeb  bl 0x1084bf24
108befd8  000050e3  cmp r0, #0x0
108befdc  00f020e3  nop
108befe0  0500000a  beq 0x108beffc
108befe4  010084e2  add r0, r4, #0x1
108befe8  ff4000e2  and r4, r0, #0xff
108befec  040055e1  cmp r5, r4
108beff0  f5ffff8a  bhi 0x108befcc
108beff4  0cd08de2  add sp, sp, #0xc
108beff8  f080bde8  ldmia sp!,{r4,r5,r6,r7,pc}
108beffc  0110a0e3  mov r1, #0x1
108bf000  4200a0e3  mov r0, #0x42
108bf004  329afeeb  bl 0x108658d4
108bf008  0cd08de2  add sp, sp, #0xc
108bf00c  f080bde8  ldmia sp!,{r4,r5,r6,r7,pc}
