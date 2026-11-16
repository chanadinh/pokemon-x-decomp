108b8a70  f05f2de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108b8a74  0300a0e3  mov r0, #0x3
108b8a78  0170a0e1  cpy r7, r1
108b8a7c  0280a0e1  cpy r8, r2
108b8a80  cc909fe5  ldr r9, [0x108b8b54]
108b8a84  a4cbfeeb  bl 0x1086b91c
108b8a88  080050e1  cmp r0, r8
108b8a8c  2f00001a  bne 0x108b8b50
108b8a90  0400a0e3  mov r0, #0x4
108b8a94  a0cbfeeb  bl 0x1086b91c
108b8a98  ff0000e2  and r0, r0, #0xff
108b8a9c  0040a0e1  cpy r4, r0
108b8aa0  0010a0e1  cpy r1, r0
108b8aa4  0700a0e1  cpy r0, r7
108b8aa8  7c6efeeb  bl 0x108544a0
108b8aac  00b0a0e1  cpy r11, r0
108b8ab0  0400a0e1  cpy r0, r4
108b8ab4  0160a0e3  mov r6, #0x1
108b8ab8  404dfeeb  bl 0x1084bfc0
108b8abc  00a0a0e1  cpy r10, r0
108b8ac0  0040a0e3  mov r4, #0x0
108b8ac4  ff5004e2  and r5, r4, #0xff
108b8ac8  0520a0e1  cpy r2, r5
108b8acc  0a10a0e1  cpy r1, r10
108b8ad0  0700a0e1  cpy r0, r7
108b8ad4  00d1ffeb  bl 0x108acedc
108b8ad8  000050e3  cmp r0, #0x0
108b8adc  00f020e3  nop
108b8ae0  0a00000a  beq 0x108b8b10
108b8ae4  0500a0e1  cpy r0, r5
108b8ae8  3e4afeeb  bl 0x1084b3e8
108b8aec  000050e3  cmp r0, #0x0
108b8af0  00f020e3  nop
108b8af4  0500000a  beq 0x108b8b10
108b8af8  000056e3  cmp r6, #0x0
108b8afc  0100001a  bne 0x108b8b08
108b8b00  100055e3  cmp r5, #0x10
108b8b04  0100000a  beq 0x108b8b10
108b8b08  0140a0e3  mov r4, #0x1
108b8b0c  030000ea  b 0x108b8b20
108b8b10  014084e2  add r4, r4, #0x1
108b8b14  110054e3  cmp r4, #0x11
108b8b18  e9ffff3a  bcc 0x108b8ac4
108b8b1c  0040a0e3  mov r4, #0x0
108b8b20  0710a0e3  mov r1, #0x7
108b8b24  0b00a0e1  cpy r0, r11
108b8b28  3a4ffeeb  bl 0x1084c818
108b8b2c  040090e1  orrs r0, r0, r4
108b8b30  00f020e3  nop
108b8b34  0500000a  beq 0x108b8b50
108b8b38  0930a0e1  cpy r3, r9
108b8b3c  0b20a0e1  cpy r2, r11
108b8b40  0810a0e1  cpy r1, r8
108b8b44  0700a0e1  cpy r0, r7
108b8b48  f05fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108b8b4c  6649feea  b 0x1084b0ec
108b8b50  f09fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,pc}
