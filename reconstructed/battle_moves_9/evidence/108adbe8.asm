108adbe8  f05f2de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108adbec  0250a0e1  cpy r5, r2
108adbf0  0170a0e1  cpy r7, r1
108adbf4  0340a0e1  cpy r4, r3
108adbf8  00b0a0e1  cpy r11, r0
108adbfc  0200a0e3  mov r0, #0x2
108adc00  45f7feeb  bl 0x1086b91c
108adc04  050050e1  cmp r0, r5
108adc08  1900001a  bne 0x108adc74
108adc0c  000094e5  ldr r0, [r4,#0x0]
108adc10  0090a0e3  mov r9, #0x0
108adc14  0180a0e3  mov r8, #0x1
108adc18  000050e3  cmp r0, #0x0
108adc1c  19a0a0e3  mov r10, #0x19
108adc20  1400000a  beq 0x108adc78
108adc24  010050e3  cmp r0, #0x1
108adc28  2800000a  beq 0x108adcd0
108adc2c  0110a0e3  mov r1, #0x1
108adc30  0700a0e1  cpy r0, r7
108adc34  af78feeb  bl 0x1084bef8
108adc38  180094e5  ldr r0, [r4,#0x18]
108adc3c  000050e3  cmp r0, #0x0
108adc40  0b00000a  beq 0x108adc74
108adc44  0520a0e1  cpy r2, r5
108adc48  0a10a0e3  mov r1, #0xa
108adc4c  0700a0e1  cpy r0, r7
108adc50  c9f0feeb  bl 0x10869f7c
108adc54  04a0c0e5  strb r10, [r0,#0x4]
108adc58  1180c0e5  strb r8, [r0,#0x11]
108adc5c  0550c0e5  strb r5, [r0,#0x5]
108adc60  0010a0e1  cpy r1, r0
108adc64  1280c0e5  strb r8, [r0,#0x12]
108adc68  0700a0e1  cpy r0, r7
108adc6c  b5e7feeb  bl 0x10867b48
108adc70  189084e5  str r9, [r4,#0x18]
108adc74  f09fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,pc}
108adc78  0520a0e1  cpy r2, r5
108adc7c  0b10a0e3  mov r1, #0xb
108adc80  0700a0e1  cpy r0, r7
108adc84  bcf0feeb  bl 0x10869f7c
108adc88  0060a0e1  cpy r6, r0
108adc8c  b4a0c0e1  strh r10, [r0,#0x4]
108adc90  0b00a0e1  cpy r0, r11
108adc94  b803d0e1  ldrh r0, [r0,#0x38]
108adc98  00f020e3  nop
108adc9c  00f020e3  nop
108adca0  c279feeb  bl 0x1084c3b0
108adca4  080086e5  str r0, [r6,#0x8]
108adca8  0610a0e1  cpy r1, r6
108adcac  0700a0e1  cpy r0, r7
108adcb0  0f50c6e5  strb r5, [r6,#0xf]
108adcb4  a3e7feeb  bl 0x10867b48
108adcb8  0210a0e3  mov r1, #0x2
108adcbc  3f00a0e3  mov r0, #0x3f
108adcc0  03dffeeb  bl 0x108658d4
108adcc4  188084e5  str r8, [r4,#0x18]
108adcc8  f080c4e1  strd r8, r9, [r4,#0x0]
108adccc  f09fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,pc}
108adcd0  0210a0e3  mov r1, #0x2
108adcd4  3f00a0e3  mov r0, #0x3f
108adcd8  fddefeeb  bl 0x108658d4
108adcdc  0200a0e3  mov r0, #0x2
108adce0  000084e5  str r0, [r4,#0x0]
108adce4  f09fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,pc}
