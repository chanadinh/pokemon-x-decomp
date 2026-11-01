10869f88  3cc19fe5  ldr r12, [0x1086a0cc]
10869f8c  70402de9  stmdb sp!,{r4,r5,r6,lr}
10869f90  0030a0e3  mov r3, #0x0
10869f94  83e0dce7  ldrb lr, [r12,r3,lsl #0x1]
10869f98  01005ee1  cmp lr, r1
10869f9c  1700001a  bne 0x1086a000
10869fa0  83308ce0  add r3, r12, r3, lsl #0x1
10869fa4  0130d3e5  ldrb r3, [r3,#0x1]
10869fa8  000053e3  cmp r3, #0x0
10869fac  1600000a  beq 0x1086a00c
10869fb0  030013e3  tst r3, #0x3
10869fb4  0200000a  beq 0x10869fc4
10869fb8  013083e2  add r3, r3, #0x1
10869fbc  030013e3  tst r3, #0x3
10869fc0  fcffff1a  bne 0x10869fb8
10869fc4  00c090e5  ldr r12, [r0,#0x0]
10869fc8  8c66a0e1  mov r6, r12, lsl #0xd
10869fcc  a6cb83e0  add r12, r3, r6, lsr #0x17
10869fd0  7d0f5ce3  cmp r12, #0x1f4
10869fd4  0c00008a  bhi 0x1086a00c
10869fd8  a6cb80e0  add r12, r0, r6, lsr #0x17
10869fdc  000053e3  cmp r3, #0x0
10869fe0  04508ce2  add r5, r12, #0x4
10869fe4  2300000a  beq 0x1086a078
10869fe8  0040a0e3  mov r4, #0x0
10869fec  01c003e2  and r12, r3, #0x1
10869ff0  01005ce3  cmp r12, #0x1
10869ff4  04e0a0e1  cpy lr, r4
10869ff8  0d00001a  bne 0x1086a034
10869ffc  080000ea  b 0x1086a024
1086a000  013083e2  add r3, r3, #0x1
1086a004  3d0053e3  cmp r3, #0x3d
1086a008  e1ffff3a  bcc 0x10869f94
1086a00c  0020a0e3  mov r2, #0x0
1086a010  0210a0e1  cpy r1, r2
1086a014  0200a0e1  cpy r0, r2
1086a018  dd58feeb  bl 0x10800394
1086a01c  0000a0e3  mov r0, #0x0
1086a020  7080bde8  ldmia sp!,{r4,r5,r6,pc}
1086a024  a66b8ee0  add r6, lr, r6, lsr #0x17
1086a028  006086e0  add r6, r6, r0
1086a02c  0140a0e3  mov r4, #0x1
1086a030  04e0c6e5  strb lr, [r6,#0x4]
1086a034  03005ce1  cmp r12, r3
1086a038  0e00002a  bcs 0x1086a078
1086a03c  006090e5  ldr r6, [r0,#0x0]
1086a040  02c08ce2  add r12, r12, #0x2
1086a044  03005ce1  cmp r12, r3
1086a048  8666a0e1  mov r6, r6, lsl #0xd
1086a04c  a66b84e0  add r6, r4, r6, lsr #0x17
1086a050  006086e0  add r6, r6, r0
1086a054  014084e2  add r4, r4, #0x1
1086a058  04e0c6e5  strb lr, [r6,#0x4]
1086a05c  006090e5  ldr r6, [r0,#0x0]
1086a060  8666a0e1  mov r6, r6, lsl #0xd
1086a064  a66b84e0  add r6, r4, r6, lsr #0x17
1086a068  006086e0  add r6, r6, r0
1086a06c  014084e2  add r4, r4, #0x1
1086a070  04e0c6e5  strb lr, [r6,#0x4]
1086a074  f0ffff3a  bcc 0x1086a03c
1086a078  0010c5e5  strb r1, [r5,#0x0]
1086a07c  001095e5  ldr r1, [r5,#0x0]
1086a080  48e09fe5  ldr lr, [0x1086a0d0]
1086a084  1fcca0e3  mov r12, #0x1f00
1086a088  0716c1e3  bic r1, r1, #0x700000
1086a08c  83e60ee0  and lr, lr, r3, lsl #0xd
1086a090  fe1ac1e3  bic r1, r1, #0xfe000
1086a094  0e1081e1  orr r1, r1, lr
1086a098  02240ce0  and r2, r12, r2, lsl #0x8
1086a09c  1f1cc1e3  bic r1, r1, #0x1f00
1086a0a0  021081e1  orr r1, r1, r2
1086a0a4  0215c1e3  bic r1, r1, #0x800000
1086a0a8  011381e3  orr r1, r1, #0x4000000
1086a0ac  001085e5  str r1, [r5,#0x0]
1086a0b0  001090e5  ldr r1, [r0,#0x0]
1086a0b4  e119a0e1  mov r1, r1, ror #0x13
1086a0b8  831b81e0  add r1, r1, r3, lsl #0x17
1086a0bc  e116a0e1  mov r1, r1, ror #0xd
1086a0c0  001080e5  str r1, [r0,#0x0]
1086a0c4  0500a0e1  cpy r0, r5
1086a0c8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
