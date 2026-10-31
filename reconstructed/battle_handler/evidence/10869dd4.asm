10869dd4  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
10869dd8  10d04de2  sub sp, sp, #0x10
10869ddc  0150a0e1  cpy r5, r1
10869de0  0290a0e1  cpy r9, r2
10869de4  c8a09fe5  ldr r10, [0x10869eb4]
10869de8  0040a0e3  mov r4, #0x0
10869dec  84118ae0  add r1, r10, r4, lsl #0x3
10869df0  b010d1e1  ldrh r1, [r1,#0x0]
10869df4  050051e1  cmp r1, r5
10869df8  2700001a  bne 0x10869e9c
10869dfc  1900d0e5  ldrb r0, [r0,#0x19]
10869e00  0080a0e1  cpy r8, r0
10869e04  0010a0e1  cpy r1, r0
10869e08  0060a0e3  mov r6, #0x0
10869e0c  0600a0e1  cpy r0, r6
10869e10  6a8dffeb  bl 0x1084d3c0
10869e14  0070b0e1  movs r7, r0
10869e18  00f020e3  nop
10869e1c  0c00000a  beq 0x10869e54
10869e20  00f020e3  nop
10869e24  b803d0e1  ldrh r0, [r0,#0x38]
10869e28  050050e1  cmp r0, r5
10869e2c  00f020e3  nop
10869e30  1c00000a  beq 0x10869ea8
10869e34  016086e2  add r6, r6, #0x1
10869e38  080056e3  cmp r6, #0x8
10869e3c  1900008a  bhi 0x10869ea8
10869e40  0700a0e1  cpy r0, r7
10869e44  498dffeb  bl 0x1084d370
10869e48  0070b0e1  movs r7, r0
10869e4c  00f020e3  nop
10869e50  f2ffff1a  bne 0x10869e20
10869e54  84018ae0  add r0, r10, r4, lsl #0x3
10869e58  041090e5  ldr r1, [r0,#0x4]
10869e5c  0c008de2  add r0, sp, #0xc
10869e60  31ff2fe1  blx r1
10869e64  000050e3  cmp r0, #0x0
10869e68  0e00000a  beq 0x10869ea8
10869e6c  0c109de5  ldr r1, [sp,#0xc]
10869e70  00808de5  str r8, [sp,#0x0]
10869e74  0020a0e3  mov r2, #0x0
10869e78  0930a0e1  cpy r3, r9
10869e7c  7110ffe6  uxth r1, r1
10869e80  f400cde1  strd r0, r1, [sp,#0x4]
10869e84  0510a0e1  cpy r1, r5
10869e88  0200a0e1  cpy r0, r2
10869e8c  4b8cffeb  bl 0x1084cfc0
10869e90  10d08de2  add sp, sp, #0x10
10869e94  0100a0e3  mov r0, #0x1
10869e98  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
10869e9c  014084e2  add r4, r4, #0x1
10869ea0  4a0f54e3  cmp r4, #0x128
10869ea4  d0ffff3a  bcc 0x10869dec
10869ea8  10d08de2  add sp, sp, #0x10
10869eac  0000a0e3  mov r0, #0x0
10869eb0  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
