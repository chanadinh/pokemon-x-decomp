108bd98c  f0402de9  stmdb sp!,{r4,r5,r6,r7,lr}
108bd990  0cd04de2  sub sp, sp, #0xc
108bd994  0160a0e1  cpy r6, r1
108bd998  0240a0e1  cpy r4, r2
108bd99c  0200a0e3  mov r0, #0x2
108bd9a0  ddb7feeb  bl 0x1086b91c
108bd9a4  ff0000e2  and r0, r0, #0xff
108bd9a8  0410a0e1  cpy r1, r4
108bd9ac  fd3afeeb  bl 0x1084c5a8
108bd9b0  000050e3  cmp r0, #0x0
108bd9b4  1a00001a  bne 0x108bda24
108bd9b8  0410a0e1  cpy r1, r4
108bd9bc  0600a0e1  cpy r0, r6
108bd9c0  6c3cfeeb  bl 0x1084cb78
108bd9c4  011c80e3  orr r1, r0, #0x100
108bd9c8  0d20a0e1  cpy r2, sp
108bd9cc  0600a0e1  cpy r0, r6
108bd9d0  9238feeb  bl 0x1084bc20
108bd9d4  005050e2  subs r5, r0, #0x0
108bd9d8  0040a0e3  mov r4, #0x0
108bd9dc  0d70a081  cpyhi r7, sp
108bd9e0  0f00009a  bls 0x108bda24
108bd9e4  0410d7e7  ldrb r1, [r7,r4]
108bd9e8  0600a0e1  cpy r0, r6
108bd9ec  ab5afeeb  bl 0x108544a0
108bd9f0  0810a0e3  mov r1, #0x8
108bd9f4  00f020e3  nop
108bd9f8  d8c7feeb  bl 0x1086f960
108bd9fc  000050e3  cmp r0, #0x0
108bda00  00f020e3  nop
108bda04  0200000a  beq 0x108bda14
108bda08  0110a0e3  mov r1, #0x1
108bda0c  4200a0e3  mov r0, #0x42
108bda10  af9ffeeb  bl 0x108658d4
108bda14  010084e2  add r0, r4, #0x1
108bda18  ff4000e2  and r4, r0, #0xff
108bda1c  050054e1  cmp r4, r5
108bda20  efffff3a  bcc 0x108bd9e4
108bda24  0cd08de2  add sp, sp, #0xc
108bda28  f080bde8  ldmia sp!,{r4,r5,r6,r7,pc}
