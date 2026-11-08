108be848  70402de9  stmdb sp!,{r4,r5,r6,lr}
108be84c  0250a0e1  cpy r5, r2
108be850  0300a0e3  mov r0, #0x3
108be854  30b4feeb  bl 0x1086b91c
108be858  ff0000e2  and r0, r0, #0xff
108be85c  0510a0e1  cpy r1, r5
108be860  5037feeb  bl 0x1084c5a8
108be864  000050e3  cmp r0, #0x0
108be868  2100001a  bne 0x108be8f4
108be86c  0500a0e3  mov r0, #0x5
108be870  29b4feeb  bl 0x1086b91c
108be874  006050e2  subs r6, r0, #0x0
108be878  0040a0e3  mov r4, #0x0
108be87c  0800009a  bls 0x108be8a4
108be880  060084e2  add r0, r4, #0x6
108be884  ff0000e2  and r0, r0, #0xff
108be888  23b4feeb  bl 0x1086b91c
108be88c  050050e1  cmp r0, r5
108be890  00f020e3  nop
108be894  0f00000a  beq 0x108be8d8
108be898  014084e2  add r4, r4, #0x1
108be89c  040056e1  cmp r6, r4
108be8a0  f6ffff8a  bhi 0x108be880
108be8a4  1200a0e3  mov r0, #0x12
108be8a8  1bb4feeb  bl 0x1086b91c
108be8ac  7040ffe6  uxth r4, r0
108be8b0  0400a0e1  cpy r0, r4
108be8b4  c607fdeb  bl 0x108007d4
108be8b8  0a0050e3  cmp r0, #0xa
108be8bc  00f020e3  nop
108be8c0  0400000a  beq 0x108be8d8
108be8c4  0400a0e1  cpy r0, r4
108be8c8  0198ffeb  bl 0x108a48d4
108be8cc  000050e3  cmp r0, #0x0
108be8d0  00f020e3  nop
108be8d4  0600000a  beq 0x108be8f4
108be8d8  2100a0e3  mov r0, #0x21
108be8dc  0eb4feeb  bl 0x1086b91c
108be8e0  010080e2  add r0, r0, #0x1
108be8e4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108be8e8  ff1000e2  and r1, r0, #0xff
108be8ec  2100a0e3  mov r0, #0x21
108be8f0  f79bfeea  b 0x108658d4
108be8f4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
