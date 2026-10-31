108a5aac  f04f2de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11,lr}
108a5ab0  14d04de2  sub sp, sp, #0x14
108a5ab4  050050e3  cmp r0, #0x5
108a5ab8  0050a0e1  cpy r5, r0
108a5abc  0190a0e1  cpy r9, r1
108a5ac0  38609de5  ldr r6, [sp,#0x38]
108a5ac4  02b0a0e1  cpy r11, r2
108a5ac8  0370a0e1  cpy r7, r3
108a5acc  0300003a  bcc 0x108a5ae0
108a5ad0  0020a0e3  mov r2, #0x0
108a5ad4  0210a0e1  cpy r1, r2
108a5ad8  0200a0e1  cpy r0, r2
108a5adc  2c6afdeb  bl 0x10800394
108a5ae0  e8a09fe5  ldr r10, [0x108a5bd0]
108a5ae4  0040a0e3  mov r4, #0x0
108a5ae8  8401dae7  ldrb r0, [r10,r4,lsl #0x3]
108a5aec  050050e1  cmp r0, r5
108a5af0  3000001a  bne 0x108a5bb8
108a5af4  0910a0e1  cpy r1, r9
108a5af8  0100a0e3  mov r0, #0x1
108a5afc  2f9efeeb  bl 0x1084d3c0
108a5b00  0080b0e1  movs r8, r0
108a5b04  00f020e3  nop
108a5b08  0900000a  beq 0x108a5b34
108a5b0c  00f020e3  nop
108a5b10  b803d0e1  ldrh r0, [r0,#0x38]
108a5b14  050050e1  cmp r0, r5
108a5b18  00f020e3  nop
108a5b1c  2800000a  beq 0x108a5bc4
108a5b20  0800a0e1  cpy r0, r8
108a5b24  119efeeb  bl 0x1084d370
108a5b28  0080b0e1  movs r8, r0
108a5b2c  00f020e3  nop
108a5b30  f5ffff1a  bne 0x108a5b0c
108a5b34  84018ae0  add r0, r10, r4, lsl #0x3
108a5b38  041090e5  ldr r1, [r0,#0x4]
108a5b3c  0c008de2  add r0, sp, #0xc
108a5b40  31ff2fe1  blx r1
108a5b44  0c109de5  ldr r1, [sp,#0xc]
108a5b48  00908de5  str r9, [sp,#0x0]
108a5b4c  0120a0e3  mov r2, #0x1
108a5b50  0030a0e3  mov r3, #0x0
108a5b54  7110ffe6  uxth r1, r1
108a5b58  f400cde1  strd r0, r1, [sp,#0x4]
108a5b5c  0510a0e1  cpy r1, r5
108a5b60  0200a0e1  cpy r0, r2
108a5b64  159dfeeb  bl 0x1084cfc0
108a5b68  0050b0e1  movs r5, r0
108a5b6c  00f020e3  nop
108a5b70  0d00000a  beq 0x108a5bac
108a5b74  000056e3  cmp r6, #0x0
108a5b78  0040a0e3  mov r4, #0x0
108a5b7c  0600009a  bls 0x108a5b9c
108a5b80  042197e7  ldr r2, [r7,r4,lsl #0x2]
108a5b84  ff1004e2  and r1, r4, #0xff
108a5b88  0500a0e1  cpy r0, r5
108a5b8c  fe9cfeeb  bl 0x1084cf8c
108a5b90  014084e2  add r4, r4, #0x1
108a5b94  040056e1  cmp r6, r4
108a5b98  f8ffff8a  bhi 0x108a5b80
108a5b9c  0b20a0e1  cpy r2, r11
108a5ba0  0610a0e3  mov r1, #0x6
108a5ba4  0500a0e1  cpy r0, r5
108a5ba8  f79cfeeb  bl 0x1084cf8c
108a5bac  14d08de2  add sp, sp, #0x14
108a5bb0  0500a0e1  cpy r0, r5
108a5bb4  f08fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,pc}
108a5bb8  014084e2  add r4, r4, #0x1
108a5bbc  050054e3  cmp r4, #0x5
108a5bc0  c8ffff3a  bcc 0x108a5ae8
108a5bc4  14d08de2  add sp, sp, #0x14
108a5bc8  0000a0e3  mov r0, #0x0
108a5bcc  f08fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,pc}
