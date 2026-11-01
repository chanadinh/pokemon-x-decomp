108bce68  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bce6c  0240a0e1  cpy r4, r2
108bce70  0350a0e1  cpy r5, r3
108bce74  0400a0e3  mov r0, #0x4
108bce78  a7bafeeb  bl 0x1086b91c
108bce7c  040050e1  cmp r0, r4
108bce80  0400001a  bne 0x108bce98
108bce84  1e00a0e3  mov r0, #0x1e
108bce88  a3bafeeb  bl 0x1086b91c
108bce8c  7000ffe6  uxth r0, r0
108bce90  070050e3  cmp r0, #0x7
108bce94  1000000a  beq 0x108bcedc
108bce98  0000a0e3  mov r0, #0x0
108bce9c  000050e3  cmp r0, #0x0
108bcea0  000085e5  str r0, [r5,#0x0]
108bcea4  0b00001a  bne 0x108bced8
108bcea8  0400a0e3  mov r0, #0x4
108bceac  9abafeeb  bl 0x1086b91c
108bceb0  040050e1  cmp r0, r4
108bceb4  00f020e3  nop
108bceb8  0400001a  bne 0x108bced0
108bcebc  1e00a0e3  mov r0, #0x1e
108bcec0  95bafeeb  bl 0x1086b91c
108bcec4  7000ffe6  uxth r0, r0
108bcec8  0b0050e3  cmp r0, #0xb
108bcecc  0800000a  beq 0x108bcef4
108bced0  0000a0e3  mov r0, #0x0
108bced4  000085e5  str r0, [r5,#0x0]
108bced8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
108bcedc  0110a0e3  mov r1, #0x1
108bcee0  4200a0e3  mov r0, #0x42
108bcee4  7aa2feeb  bl 0x108658d4
108bcee8  00f020e3  nop
108bceec  00f020e3  nop
108bcef0  e9ffffea  b 0x108bce9c
108bcef4  0110a0e3  mov r1, #0x1
108bcef8  4200a0e3  mov r0, #0x42
108bcefc  74a2feeb  bl 0x108658d4
108bcf00  00f020e3  nop
108bcf04  00f020e3  nop
108bcf08  f1ffffea  b 0x108bced4
