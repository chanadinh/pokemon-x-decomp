108a977c  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108a9780  0160a0e1  cpy r6, r1
108a9784  0080a0e1  cpy r8, r0
108a9788  0100a0e1  cpy r0, r1
108a978c  0270a0e1  cpy r7, r2
108a9790  0210a0e1  cpy r1, r2
108a9794  41abfeeb  bl 0x108544a0
108a9798  0090a0e1  cpy r9, r0
108a979c  5201d0e5  ldrb r0, [r0,#0x152]
108a97a0  005050e2  subs r5, r0, #0x0
108a97a4  0040a0e3  mov r4, #0x0
108a97a8  0800009a  bls 0x108a97d0
108a97ac  ff1004e2  and r1, r4, #0xff
108a97b0  0900a0e1  cpy r0, r9
108a97b4  4aa2feeb  bl 0x108520e4
108a97b8  000050e3  cmp r0, #0x0
108a97bc  00f020e3  nop
108a97c0  0300000a  beq 0x108a97d4
108a97c4  014084e2  add r4, r4, #0x1
108a97c8  040055e1  cmp r5, r4
108a97cc  f6ffff8a  bhi 0x108a97ac
108a97d0  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
108a97d4  ff0004e2  and r0, r4, #0xff
108a97d8  040050e3  cmp r0, #0x4
108a97dc  fbffff0a  beq 0x108a97d0
108a97e0  0800a0e1  cpy r0, r8
108a97e4  608cfeeb  bl 0x1084c96c
108a97e8  0720a0e1  cpy r2, r7
108a97ec  0600a0e1  cpy r0, r6
108a97f0  f047bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108a97f4  0010a0e3  mov r1, #0x0
108a97f8  528cfeea  b 0x1084c948
