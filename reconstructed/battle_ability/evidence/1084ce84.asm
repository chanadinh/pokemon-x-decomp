1084ce84  70402de9  stmdb sp!,{r4,r5,r6,lr}
1084ce88  0050b0e1  movs r5, r0
1084ce8c  01005513  cmpne r5, #0x1
1084ce90  0160a0e1  cpy r6, r1
1084ce94  0b00000a  beq 0x1084cec8
1084ce98  d0409fe5  ldr r4, [0x1084cf70]
1084ce9c  000094e5  ldr r0, [r4,#0x0]
1084cea0  600050e3  cmp r0, #0x60
1084cea4  0e00002a  bcs 0x1084cee4
1084cea8  801084e0  add r1, r4, r0, lsl #0x1
1084ceac  b410d1e1  ldrh r1, [r1,#0x4]
1084ceb0  000051e3  cmp r1, #0x0
1084ceb4  0a00000a  beq 0x1084cee4
1084ceb8  050051e1  cmp r1, r5
1084cebc  01008012  addne r0, r0, #0x1
1084cec0  f6ffff1a  bne 0x1084cea0
1084cec4  0b0000ea  b 0x1084cef8
1084cec8  0020a0e3  mov r2, #0x0
1084cecc  0210a0e1  cpy r1, r2
1084ced0  0200a0e1  cpy r0, r2
1084ced4  2ecdfeeb  bl 0x10800394
1084ced8  00f020e3  nop
1084cedc  00f020e3  nop
1084cee0  ecffffea  b 0x1084ce98
1084cee4  0020a0e3  mov r2, #0x0
1084cee8  0210a0e1  cpy r1, r2
1084ceec  0200a0e1  cpy r0, r2
1084cef0  27cdfeeb  bl 0x10800394
1084cef4  0000e0e3  mvn r0, #0x0
1084cef8  000050e3  cmp r0, #0x0
1084cefc  1a0000ba  blt 0x1084cf6c
1084cf00  001084e0  add r1, r4, r0
1084cf04  4415d1e5  ldrb r1, [r1,#0x544]
1084cf08  030051e3  cmp r1, #0x3
1084cf0c  0400000a  beq 0x1084cf24
1084cf10  0020a0e3  mov r2, #0x0
1084cf14  7040bde8  ldmia sp!,{r4,r5,r6,lr}
1084cf18  0210a0e1  cpy r1, r2
1084cf1c  0200a0e1  cpy r0, r2
1084cf20  1bcdfeea  b 0x10800394
1084cf24  001184e0  add r1, r4, r0, lsl #0x2
1084cf28  c40091e5  ldr r0, [r1,#0xc4]
1084cf2c  9006c2e0  smull r0, r2, r0, r6
1084cf30  020b90e2  adds r0, r0, #0x800
1084cf34  0020a2e2  adc r2, r2, #0x0
1084cf38  2006a0e1  mov r0, r0, lsr #0xc
1084cf3c  020a80e1  orr r0, r0, r2, lsl #0x14
1084cf40  442291e5  ldr r2, [r1,#0x244]
1084cf44  000052e3  cmp r2, #0x0
1084cf48  c4339105  ldreq r3, [r1,#0x3c4]
1084cf4c  00005303  cmpeq r3, #0x0
1084cf50  0400000a  beq 0x1084cf68
1084cf54  c43391e5  ldr r3, [r1,#0x3c4]
1084cf58  000053e1  cmp r3, r0
1084cf5c  0300a0c1  cpygt r0, r3
1084cf60  000052e1  cmp r2, r0
1084cf64  0200a0b1  cpylt r0, r2
1084cf68  c40081e5  str r0, [r1,#0xc4]
1084cf6c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
