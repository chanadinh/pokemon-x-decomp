0013ff50  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
0013ff54  00c0a0e3  mov r12, #0x0
0013ff58  18409de5  ldr r4, [sp,#0x18]
0013ff5c  dce0d4e1  ldrsb lr, [r4,#0xc]
0013ff60  00005ee3  cmp lr, #0x0
0013ff64  0200000a  beq 0x0013ff74
0013ff68  04e094e5  ldr lr, [r4,#0x4]
0013ff6c  b0c0cee1  strh r12, [lr,#0x0]
0013ff70  bac0c4e1  strh r12, [r4,#0xa]
0013ff74  180090e5  ldr r0, [r0,#0x18]
0013ff78  080090e5  ldr r0, [r0,#0x8]
0013ff7c  000050e3  cmp r0, #0x0
0013ff80  1400000a  beq 0x0013ffd8
0013ff84  000052e3  cmp r2, #0x0
0013ff88  00e0a0e3  mov lr, #0x0
0013ff8c  4800009a  bls 0x001400b4
0013ff90  dc30d4e1  ldrsb r3, [r4,#0xc]
0013ff94  b000d1e1  ldrh r0, [r1,#0x0]
0013ff98  000053e3  cmp r3, #0x0
0013ff9c  3b00000a  beq 0x00140090
0013ffa0  ba50d4e1  ldrh r5, [r4,#0xa]
0013ffa4  b860d4e1  ldrh r6, [r4,#0x8]
0013ffa8  013085e2  add r3, r5, #0x1
0013ffac  060053e1  cmp r3, r6
0013ffb0  3100002a  bcs 0x0014007c
0013ffb4  046094e5  ldr r6, [r4,#0x4]
0013ffb8  8570a0e1  mov r7, r5, lsl #0x1
0013ffbc  ba30c4e1  strh r3, [r4,#0xa]
0013ffc0  b70086e1  strh r0, [r6,r7]
0013ffc4  ba00d4e1  ldrh r0, [r4,#0xa]
0013ffc8  043094e5  ldr r3, [r4,#0x4]
0013ffcc  800083e0  add r0, r3, r0, lsl #0x1
0013ffd0  b0c0c0e1  strh r12, [r0,#0x0]
0013ffd4  320000ea  b 0x001400a4
0013ffd8  f0009fe5  ldr r0, [0x1400d0]
0013ffdc  033083e2  add r3, r3, #0x3
0013ffe0  000052e3  cmp r2, #0x0
0013ffe4  0050a0e3  mov r5, #0x0
0013ffe8  930000e0  mul r0, r3, r0
0013ffec  7000ffe6  uxth r0, r0
0013fff0  2f00009a  bls 0x001400b4
0013fff4  b030d1e1  ldrh r3, [r1,#0x0]
0013fff8  dce0d4e1  ldrsb lr, [r4,#0xc]
0013fffc  003023e0  eor r3, r3, r0
00140000  00005ee3  cmp lr, #0x0
00140004  2100000a  beq 0x00140090
00140008  ba60d4e1  ldrh r6, [r4,#0xa]
0014000c  b870d4e1  ldrh r7, [r4,#0x8]
00140010  01e086e2  add lr, r6, #0x1
00140014  07005ee1  cmp lr, r7
00140018  0800002a  bcs 0x00140040
0014001c  047094e5  ldr r7, [r4,#0x4]
00140020  8660a0e1  mov r6, r6, lsl #0x1
00140024  bae0c4e1  strh lr, [r4,#0xa]
00140028  b63087e1  strh r3, [r7,r6]
0014002c  ba30d4e1  ldrh r3, [r4,#0xa]
00140030  04e094e5  ldr lr, [r4,#0x4]
00140034  83308ee0  add r3, lr, r3, lsl #0x1
00140038  b0c0c3e1  strh r12, [r3,#0x0]
0014003c  050000ea  b 0x00140058
00140040  00005303  cmpeq r3, #0x0
00140044  0e00001a  bne 0x00140084
00140048  047094e5  ldr r7, [r4,#0x4]
0014004c  8660a0e1  mov r6, r6, lsl #0x1
00140050  bae0c4e1  strh lr, [r4,#0xa]
00140054  b6c087e1  strh r12, [r7,r6]
00140058  0e3a00e2  and r3, r0, #0xe000
0014005c  0e0ac0e3  bic r0, r0, #0xe000
00140060  a336a0e1  mov r3, r3, lsr #0xd
00140064  015085e2  add r5, r5, #0x1
00140068  020055e1  cmp r5, r2
0014006c  021081e2  add r1, r1, #0x2
00140070  800183e1  orr r0, r3, r0, lsl #0x3
00140074  deffff3a  bcc 0x0013fff4
00140078  0d0000ea  b 0x001400b4
0014007c  00005003  cmpeq r0, #0x0
00140080  0300000a  beq 0x00140094
00140084  040094e5  ldr r0, [r4,#0x4]
00140088  b0c0c0e1  strh r12, [r0,#0x0]
0014008c  bac0c4e1  strh r12, [r4,#0xa]
00140090  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
00140094  046094e5  ldr r6, [r4,#0x4]
00140098  8550a0e1  mov r5, r5, lsl #0x1
0014009c  ba30c4e1  strh r3, [r4,#0xa]
001400a0  b5c086e1  strh r12, [r6,r5]
001400a4  01e08ee2  add lr, lr, #0x1
001400a8  02005ee1  cmp lr, r2
001400ac  021081e2  add r1, r1, #0x2
001400b0  b6ffff3a  bcc 0x0013ff90
001400b4  0c00d4e5  ldrb r0, [r4,#0xc]
001400b8  000050e3  cmp r0, #0x0
001400bc  f3ffff0a  beq 0x00140090
001400c0  040094e5  ldr r0, [r4,#0x4]
001400c4  2e3a00eb  bl 0x0014e984
001400c8  ba00c4e1  strh r0, [r4,#0xa]
001400cc  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
