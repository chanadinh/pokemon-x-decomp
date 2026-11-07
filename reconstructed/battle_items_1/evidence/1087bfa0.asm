1087bfa0  70402de9  stmdb sp!,{r4,r5,r6,lr}
1087bfa4  0040b0e1  movs r4, r0
1087bfa8  01005413  cmpne r4, #0x1
1087bfac  0150a0e1  cpy r5, r1
1087bfb0  0c00000a  beq 0x1087bfe8
1087bfb4  64109fe5  ldr r1, [0x1087c020]
1087bfb8  000091e5  ldr r0, [r1,#0x0]
1087bfbc  600050e3  cmp r0, #0x60
1087bfc0  1400002a  bcs 0x1087c018
1087bfc4  802081e0  add r2, r1, r0, lsl #0x1
1087bfc8  b420d2e1  ldrh r2, [r2,#0x4]
1087bfcc  040052e1  cmp r2, r4
1087bfd0  0b00001a  bne 0x1087c004
1087bfd4  000181e0  add r0, r1, r0, lsl #0x2
1087bfd8  c40090e5  ldr r0, [r0,#0xc4]
1087bfdc  000085e5  str r0, [r5,#0x0]
1087bfe0  0100a0e3  mov r0, #0x1
1087bfe4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
1087bfe8  0020a0e3  mov r2, #0x0
1087bfec  0210a0e1  cpy r1, r2
1087bff0  0200a0e1  cpy r0, r2
1087bff4  e610feeb  bl 0x10800394
1087bff8  00f020e3  nop
1087bffc  00f020e3  nop
1087c000  ebffffea  b 0x1087bfb4
1087c004  000052e3  cmp r2, #0x0
1087c008  0200000a  beq 0x1087c018
1087c00c  010080e2  add r0, r0, #0x1
1087c010  600050e3  cmp r0, #0x60
1087c014  eaffff3a  bcc 0x1087bfc4
1087c018  0000a0e3  mov r0, #0x0
1087c01c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
