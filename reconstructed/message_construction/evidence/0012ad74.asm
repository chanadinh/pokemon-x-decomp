0012ad74  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
0012ad78  10d04de2  sub sp, sp, #0x10
0012ad7c  0040a0e1  cpy r4, r0
0012ad80  0170a0e1  cpy r7, r1
0012ad84  08a090e5  ldr r10, [r0,#0x8]
0012ad88  6c819fe5  ldr r8, [0x12aefc]
0012ad8c  0260a0e1  cpy r6, r2
0012ad90  00005ae3  cmp r10, #0x0
0012ad94  5100000a  beq 0x0012aee0
0012ad98  0090a0e3  mov r9, #0x0
0012ad9c  b492c4e1  strh r9, [r4,#0x24]
0012ada0  209084e5  str r9, [r4,#0x20]
0012ada4  0920a0e1  cpy r2, r9
0012ada8  0a00a0e1  cpy r0, r10
0012adac  b692c4e1  strh r9, [r4,#0x26]
0012adb0  033500eb  bl 0x001381c4
0012adb4  0d20a0e1  cpy r2, sp
0012adb8  0c10a0e3  mov r1, #0xc
0012adbc  0a00a0e1  cpy r0, r10
0012adc0  69f3ffeb  bl 0x00127b6c
0012adc4  b000dde1  ldrh r0, [sp,#0x0]
0012adc8  0c10a0e3  mov r1, #0xc
0012adcc  005181e0  add r5, r1, r0, lsl #0x2
0012add0  000096e5  ldr r0, [r6,#0x0]
0012add4  1c1090e5  ldr r1, [r0,#0x1c]
0012add8  0600a0e1  cpy r0, r6
0012addc  31ff2fe1  blx r1
0012ade0  0020a0e1  cpy r2, r0
0012ade4  0130a0e3  mov r3, #0x1
0012ade8  0510a0e1  cpy r1, r5
0012adec  0600a0e1  cpy r0, r6
0012adf0  b40000eb  bl 0x0012b0c8
0012adf4  0050b0e1  movs r5, r0
0012adf8  0600000a  beq 0x0012ae18
0012adfc  07009de8  ldmia sp,{r0,r1,r2}
0012ae00  070085e8  stmia r5,{r0,r1,r2}
0012ae04  0c2085e2  add r2, r5, #0xc
0012ae08  b000d5e1  ldrh r0, [r5,#0x0]
0012ae0c  0011a0e1  mov r1, r0, lsl #0x2
0012ae10  0a00a0e1  cpy r0, r10
0012ae14  54f3ffeb  bl 0x00127b6c
0012ae18  185084e5  str r5, [r4,#0x18]
0012ae1c  107084e5  str r7, [r4,#0x10]
0012ae20  b010d5e1  ldrh r1, [r5,#0x0]
0012ae24  640098e5  ldr r0, [r8,#0x64]
0012ae28  000051e1  cmp r1, r0
0012ae2c  0c10d4e5  ldrb r1, [r4,#0xc]
0012ae30  0000a093  movls r0, #0x0
0012ae34  140084e5  str r0, [r4,#0x14]
0012ae38  000051e3  cmp r1, #0x0
0012ae3c  1c908405  streq r9, [r4,#0x1c]
0012ae40  2b00000a  beq 0x0012aef4
0012ae44  000096e5  ldr r0, [r6,#0x0]
0012ae48  045095e5  ldr r5, [r5,#0x4]
0012ae4c  1c1090e5  ldr r1, [r0,#0x1c]
0012ae50  0600a0e1  cpy r0, r6
0012ae54  31ff2fe1  blx r1
0012ae58  0020a0e1  cpy r2, r0
0012ae5c  0130a0e3  mov r3, #0x1
0012ae60  0510a0e1  cpy r1, r5
0012ae64  0600a0e1  cpy r0, r6
0012ae68  960000eb  bl 0x0012b0c8
0012ae6c  1c0084e5  str r0, [r4,#0x1c]
0012ae70  141094e5  ldr r1, [r4,#0x14]
0012ae74  180094e5  ldr r0, [r4,#0x18]
0012ae78  085094e5  ldr r5, [r4,#0x8]
0012ae7c  107094e5  ldr r7, [r4,#0x10]
0012ae80  011180e0  add r1, r0, r1, lsl #0x2
0012ae84  000055e3  cmp r5, #0x0
0012ae88  0c6091e5  ldr r6, [r1,#0xc]
0012ae8c  06008000  addeq r0, r0, r6
0012ae90  1c008405  streq r0, [r4,#0x1c]
0012ae94  1600000a  beq 0x0012aef4
0012ae98  0620a0e1  cpy r2, r6
0012ae9c  0710a0e1  cpy r1, r7
0012aea0  0500a0e1  cpy r0, r5
0012aea4  c63400eb  bl 0x001381c4
0012aea8  1c2094e5  ldr r2, [r4,#0x1c]
0012aeac  0410a0e3  mov r1, #0x4
0012aeb0  0500a0e1  cpy r0, r5
0012aeb4  2cf3ffeb  bl 0x00127b6c
0012aeb8  0620a0e1  cpy r2, r6
0012aebc  0710a0e1  cpy r1, r7
0012aec0  0500a0e1  cpy r0, r5
0012aec4  be3400eb  bl 0x001381c4
0012aec8  1c2094e5  ldr r2, [r4,#0x1c]
0012aecc  0500a0e1  cpy r0, r5
0012aed0  001092e5  ldr r1, [r2,#0x0]
0012aed4  10d08de2  add sp, sp, #0x10
0012aed8  f047bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
0012aedc  22f3ffea  b 0x00127b6c
0012aee0  6c1098e5  ldr r1, [r8,#0x6c]
0012aee4  000051e3  cmp r1, #0x0
0012aee8  10009f15  ldrne r0, [0x12af00]
0012aeec  31ff2f11  blxne r1
0012aef0  faffffea  b 0x0012aee0
0012aef4  10d08de2  add sp, sp, #0x10
0012aef8  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
