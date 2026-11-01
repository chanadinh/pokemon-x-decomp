108bf390  ff4f2de9  stmdb sp!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,lr}
108bf394  0cd04de2  sub sp, sp, #0xc
108bf398  0200a0e3  mov r0, #0x2
108bf39c  0170a0e1  cpy r7, r1
108bf3a0  5db1feeb  bl 0x1086b91c
108bf3a4  14109de5  ldr r1, [sp,#0x14]
108bf3a8  010050e1  cmp r0, r1
108bf3ac  6600001a  bne 0x108bf54c
108bf3b0  0d20a0e1  cpy r2, sp
108bf3b4  0700a0e1  cpy r0, r7
108bf3b8  172efeeb  bl 0x1084ac1c
108bf3bc  00b0a0e1  cpy r11, r0
108bf3c0  14109de5  ldr r1, [sp,#0x14]
108bf3c4  0700a0e1  cpy r0, r7
108bf3c8  3454feeb  bl 0x108544a0
108bf3cc  00005be3  cmp r11, #0x0
108bf3d0  0090a0e1  cpy r9, r0
108bf3d4  0060a0e3  mov r6, #0x0
108bf3d8  4200009a  bls 0x108bf4e8
108bf3dc  0610dde7  ldrb r1, [sp,r6]
108bf3e0  0700a0e1  cpy r0, r7
108bf3e4  2d54feeb  bl 0x108544a0
108bf3e8  00a0a0e1  cpy r10, r0
108bf3ec  0900a0e1  cpy r0, r9
108bf3f0  67c5feeb  bl 0x10870994
108bf3f4  08008de5  str r0, [sp,#0x8]
108bf3f8  0a00a0e1  cpy r0, r10
108bf3fc  5201d0e5  ldrb r0, [r0,#0x152]
108bf400  005050e2  subs r5, r0, #0x0
108bf404  0040a0e3  mov r4, #0x0
108bf408  3200009a  bls 0x108bf4d8
108bf40c  0410a0e1  cpy r1, r4
108bf410  0a00a0e1  cpy r0, r10
108bf414  135afeeb  bl 0x10855c68
108bf418  0080a0e1  cpy r8, r0
108bf41c  00f020e3  nop
108bf420  eb04fdeb  bl 0x108007d4
108bf424  090050e3  cmp r0, #0x9
108bf428  00f020e3  nop
108bf42c  2d00000a  beq 0x108bf4e8
108bf430  0800a0e1  cpy r0, r8
108bf434  fe03fdeb  bl 0x10800434
108bf438  000050e3  cmp r0, #0x0
108bf43c  00f020e3  nop
108bf440  2000000a  beq 0x108bf4c8
108bf444  ed0058e3  cmp r8, #0xed
108bf448  0400000a  beq 0x108bf460
108bf44c  0800a0e1  cpy r0, r8
108bf450  f903fdeb  bl 0x1080043c
108bf454  00f020e3  nop
108bf458  00f020e3  nop
108bf45c  140000ea  b 0x108bf4b4
108bf460  0780a0e1  cpy r8, r7
108bf464  0a00a0e1  cpy r0, r10
108bf468  1900d0e5  ldrb r0, [r0,#0x19]
108bf46c  0010a0e1  cpy r1, r0
108bf470  0800a0e1  cpy r0, r8
108bf474  0954feeb  bl 0x108544a0
108bf478  000050e3  cmp r0, #0x0
108bf47c  00f020e3  nop
108bf480  0600000a  beq 0x108bf4a0
108bf484  000090e5  ldr r0, [r0,#0x0]
108bf488  00f020e3  nop
108bf48c  00f020e3  nop
108bf490  9909fdeb  bl 0x10801afc
108bf494  00f020e3  nop
108bf498  00f020e3  nop
108bf49c  040000ea  b 0x108bf4b4
108bf4a0  0020a0e3  mov r2, #0x0
108bf4a4  0210a0e1  cpy r1, r2
108bf4a8  0200a0e1  cpy r0, r2
108bf4ac  b803fdeb  bl 0x10800394
108bf4b0  0000a0e3  mov r0, #0x0
108bf4b4  08109de5  ldr r1, [sp,#0x8]
108bf4b8  10c5feeb  bl 0x10870900
108bf4bc  070050e3  cmp r0, #0x7
108bf4c0  00f020e3  nop
108bf4c4  0700008a  bhi 0x108bf4e8
108bf4c8  010084e2  add r0, r4, #0x1
108bf4cc  ff4000e2  and r4, r0, #0xff
108bf4d0  040055e1  cmp r5, r4
108bf4d4  ccffff8a  bhi 0x108bf40c
108bf4d8  010086e2  add r0, r6, #0x1
108bf4dc  ff6000e2  and r6, r0, #0xff
108bf4e0  06005be1  cmp r11, r6
108bf4e4  bcffff8a  bhi 0x108bf3dc
108bf4e8  0b0056e1  cmp r6, r11
108bf4ec  1600000a  beq 0x108bf54c
108bf4f0  14209de5  ldr r2, [sp,#0x14]
108bf4f4  0110a0e3  mov r1, #0x1
108bf4f8  0700a0e1  cpy r0, r7
108bf4fc  1135feeb  bl 0x1084c948
108bf500  14209de5  ldr r2, [sp,#0x14]
108bf504  0310a0e3  mov r1, #0x3
108bf508  0700a0e1  cpy r0, r7
108bf50c  9aaafeeb  bl 0x10869f7c
108bf510  0040a0e1  cpy r4, r0
108bf514  6d2fa0e3  mov r2, #0x1b4
108bf518  0210a0e3  mov r1, #0x2
108bf51c  040080e2  add r0, r0, #0x4
108bf520  6b37feeb  bl 0x1084d2d4
108bf524  14109de5  ldr r1, [sp,#0x14]
108bf528  040084e2  add r0, r4, #0x4
108bf52c  5a37feeb  bl 0x1084d29c
108bf530  0410a0e1  cpy r1, r4
108bf534  0700a0e1  cpy r0, r7
108bf538  82a1feeb  bl 0x10867b48
108bf53c  14209de5  ldr r2, [sp,#0x14]
108bf540  0210a0e3  mov r1, #0x2
108bf544  0700a0e1  cpy r0, r7
108bf548  fe34feeb  bl 0x1084c948
108bf54c  1cd08de2  add sp, sp, #0x1c
108bf550  f08fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,pc}
