1081fc20  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
1081fc24  0050a0e1  cpy r5, r0
1081fc28  010c81e2  add r0, r1, #0x100
1081fc2c  0140a0e1  cpy r4, r1
1081fc30  0370a0e1  cpy r7, r3
1081fc34  690080e2  add r0, r0, #0x69
1081fc38  a211a0e1  mov r1, r2, lsr #0x3
1081fc3c  073002e2  and r3, r2, #0x7
1081fc40  0180a0e3  mov r8, #0x1
1081fc44  01c0d0e7  ldrb r12, [r0,r1]
1081fc48  1833a0e1  mov r3, r8, lsl r3
1081fc4c  ff6003e2  and r6, r3, #0xff
1081fc50  0c0016e1  tst r6, r12
1081fc54  0000a003  moveq r0, #0x0
1081fc58  3300000a  beq 0x1081fd2c
1081fc5c  ff1001e2  and r1, r1, #0xff
1081fc60  1960d4e5  ldrb r6, [r4,#0x19]
1081fc64  0330cce1  bic r3, r12, r3
1081fc68  0130c0e7  strb r3, [r0,r1]
1081fc6c  1910d4e5  ldrb r1, [r4,#0x19]
1081fc70  0230a0e1  cpy r3, r2
1081fc74  0c0095e5  ldr r0, [r5,#0xc]
1081fc78  0120a0e1  cpy r2, r1
1081fc7c  1c10a0e3  mov r1, #0x1c
1081fc80  693101eb  bl 0x1086c22c
1081fc84  840094e5  ldr r0, [r4,#0x84]
1081fc88  070010e3  tst r0, #0x7
1081fc8c  0400000a  beq 0x1081fca4
1081fc90  0030a0e3  mov r3, #0x0
1081fc94  1a20a0e3  mov r2, #0x1a
1081fc98  0410a0e1  cpy r1, r4
1081fc9c  0500a0e1  cpy r0, r5
1081fca0  462601eb  bl 0x108695c0
1081fca4  0c0095e5  ldr r0, [r5,#0xc]
1081fca8  0030a0e3  mov r3, #0x0
1081fcac  0620a0e1  cpy r2, r6
1081fcb0  3810a0e3  mov r1, #0x38
1081fcb4  5c3101eb  bl 0x1086c22c
1081fcb8  000057e3  cmp r7, #0x0
1081fcbc  00f020e3  nop
1081fcc0  1800001a  bne 0x1081fd28
1081fcc4  74109fe5  ldr r1, [0x1081fd40]
1081fcc8  0120a0e3  mov r2, #0x1
1081fccc  0000a0e3  mov r0, #0x0
1081fcd0  0530d1e7  ldrb r3, [r1,r5]
1081fcd4  000053e3  cmp r3, #0x0
1081fcd8  1200009a  bls 0x1081fd28
1081fcdc  010000ea  b 0x1081fce8
1081fce0  000052e3  cmp r2, #0x0
1081fce4  0500000a  beq 0x1081fd00
1081fce8  001285e0  add r1, r5, r0, lsl #0x4
1081fcec  431d81e2  add r1, r1, #0x10c0
1081fcf0  001091e5  ldr r1, [r1,#0x0]
1081fcf4  0f1001e2  and r1, r1, #0xf
1081fcf8  080051e3  cmp r1, #0x8
1081fcfc  0b00000a  beq 0x1081fd30
1081fd00  001285e0  add r1, r5, r0, lsl #0x4
1081fd04  011a81e2  add r1, r1, #0x1000
1081fd08  bc1091e5  ldr r1, [r1,#0xbc]
1081fd0c  1910d1e5  ldrb r1, [r1,#0x19]
1081fd10  060051e1  cmp r1, r6
1081fd14  0500001a  bne 0x1081fd30
1081fd18  000285e0  add r0, r5, r0, lsl #0x4
1081fd1c  010a80e2  add r0, r0, #0x1000
1081fd20  bc0090e2  adds r0, r0, #0xbc
1081fd24  0d80c015  strbne r8, [r0,#0xd]
1081fd28  0100a0e3  mov r0, #0x1
1081fd2c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
1081fd30  010080e2  add r0, r0, #0x1
1081fd34  000053e1  cmp r3, r0
1081fd38  e8ffff8a  bhi 0x1081fce0
1081fd3c  f9ffffea  b 0x1081fd28
