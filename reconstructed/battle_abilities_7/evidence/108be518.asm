108be518  f0402de9  stmdb sp!,{r4,r5,r6,r7,lr}
108be51c  0cd04de2  sub sp, sp, #0xc
108be520  0160a0e1  cpy r6, r1
108be524  0240a0e1  cpy r4, r2
108be528  0200a0e3  mov r0, #0x2
108be52c  fab4feeb  bl 0x1086b91c
108be530  ff0000e2  and r0, r0, #0xff
108be534  0410a0e1  cpy r1, r4
108be538  1a38feeb  bl 0x1084c5a8
108be53c  000050e3  cmp r0, #0x0
108be540  1a00001a  bne 0x108be5b0
108be544  0410a0e1  cpy r1, r4
108be548  0600a0e1  cpy r0, r6
108be54c  8939feeb  bl 0x1084cb78
108be550  011c80e3  orr r1, r0, #0x100
108be554  0d20a0e1  cpy r2, sp
108be558  0600a0e1  cpy r0, r6
108be55c  af35feeb  bl 0x1084bc20
108be560  005050e2  subs r5, r0, #0x0
108be564  0040a0e3  mov r4, #0x0
108be568  0d70a081  cpyhi r7, sp
108be56c  0c00009a  bls 0x108be5a4
108be570  0410d7e7  ldrb r1, [r7,r4]
108be574  0600a0e1  cpy r0, r6
108be578  c857feeb  bl 0x108544a0
108be57c  1110a0e3  mov r1, #0x11
108be580  00f020e3  nop
108be584  a5d7feeb  bl 0x10874420
108be588  170050e3  cmp r0, #0x17
108be58c  00f020e3  nop
108be590  0600000a  beq 0x108be5b0
108be594  010084e2  add r0, r4, #0x1
108be598  ff4000e2  and r4, r0, #0xff
108be59c  040055e1  cmp r5, r4
108be5a0  f2ffff8a  bhi 0x108be570
108be5a4  0110a0e3  mov r1, #0x1
108be5a8  4200a0e3  mov r0, #0x42
108be5ac  c89cfeeb  bl 0x108658d4
108be5b0  0cd08de2  add sp, sp, #0xc
108be5b4  f080bde8  ldmia sp!,{r4,r5,r6,r7,pc}
