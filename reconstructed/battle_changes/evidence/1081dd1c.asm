1081dd1c  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
1081dd20  0050a0e1  cpy r5, r0
1081dd24  0140a0e1  cpy r4, r1
1081dd28  010a80e2  add r0, r0, #0x1000
1081dd2c  0610d1e5  ldrb r1, [r1,#0x6]
1081dd30  08d04de2  sub sp, sp, #0x8
1081dd34  830f80e2  add r0, r0, #0x20c
1081dd38  633901eb  bl 0x1086c2cc
1081dd3c  0a0050e3  cmp r0, #0xa
1081dd40  6a00000a  beq 0x1081def0
1081dd44  0660d4e5  ldrb r6, [r4,#0x6]
1081dd48  087095e5  ldr r7, [r5,#0x8]
1081dd4c  180056e3  cmp r6, #0x18
1081dd50  0300003a  bcc 0x1081dd64
1081dd54  0020a0e3  mov r2, #0x0
1081dd58  0210a0e1  cpy r1, r2
1081dd5c  0200a0e1  cpy r0, r2
1081dd60  8b89ffeb  bl 0x10800394
1081dd64  066187e0  add r6, r7, r6, lsl #0x2
1081dd68  840096e5  ldr r0, [r6,#0x84]
1081dd6c  000050e3  cmp r0, #0x0
1081dd70  0300001a  bne 0x1081dd84
1081dd74  0020a0e3  mov r2, #0x0
1081dd78  0210a0e1  cpy r1, r2
1081dd7c  0200a0e1  cpy r0, r2
1081dd80  8389ffeb  bl 0x10800394
1081dd84  000094e5  ldr r0, [r4,#0x0]
1081dd88  846096e5  ldr r6, [r6,#0x84]
1081dd8c  1f10a0e3  mov r1, #0x1f
1081dd90  0070a0e3  mov r7, #0x0
1081dd94  8009a0e1  mov r0, r0, lsl #0x13
1081dd98  a00d51e1  cmp r1, r0, lsr #0x1b
1081dd9c  1000000a  beq 0x1081dde4
1081dda0  088095e5  ldr r8, [r5,#0x8]
1081dda4  a07da0e1  mov r7, r0, lsr #0x1b
1081dda8  180057e3  cmp r7, #0x18
1081ddac  0300003a  bcc 0x1081ddc0
1081ddb0  0020a0e3  mov r2, #0x0
1081ddb4  0210a0e1  cpy r1, r2
1081ddb8  0200a0e1  cpy r0, r2
1081ddbc  7489ffeb  bl 0x10800394
1081ddc0  077188e0  add r7, r8, r7, lsl #0x2
1081ddc4  840097e5  ldr r0, [r7,#0x84]
1081ddc8  000050e3  cmp r0, #0x0
1081ddcc  0300001a  bne 0x1081dde0
1081ddd0  0020a0e3  mov r2, #0x0
1081ddd4  0210a0e1  cpy r1, r2
1081ddd8  0200a0e1  cpy r0, r2
1081dddc  6c89ffeb  bl 0x10800394
1081dde0  847097e5  ldr r7, [r7,#0x84]
1081dde4  0600a0e1  cpy r0, r6
1081dde8  0d10a0e3  mov r1, #0xd
1081ddec  8b5901eb  bl 0x10874420
1081ddf0  000050e3  cmp r0, #0x0
1081ddf4  00f020e3  nop
1081ddf8  3c00000a  beq 0x1081def0
1081ddfc  0700d4e5  ldrb r0, [r4,#0x7]
1081de00  010010e3  tst r0, #0x1
1081de04  0400000a  beq 0x1081de1c
1081de08  0600a0e1  cpy r0, r6
1081de0c  4d5901eb  bl 0x10874348
1081de10  120050e3  cmp r0, #0x12
1081de14  00f020e3  nop
1081de18  3400001a  bne 0x1081def0
1081de1c  00f020e3  nop
1081de20  573701eb  bl 0x1086bb84
1081de24  1910d6e5  ldrb r1, [r6,#0x19]
1081de28  0400a0e3  mov r0, #0x4
1081de2c  233701eb  bl 0x1086bac0
1081de30  0110a0e3  mov r1, #0x1
1081de34  5300a0e3  mov r0, #0x53
1081de38  ef3601eb  bl 0x1086b9fc
1081de3c  8910a0e3  mov r1, #0x89
1081de40  0500a0e1  cpy r0, r5
1081de44  d63601eb  bl 0x1086b9a4
1081de48  5300a0e3  mov r0, #0x53
1081de4c  00f020e3  nop
1081de50  b13601eb  bl 0x1086b91c
1081de54  000050e3  cmp r0, #0x0
1081de58  00f020e3  nop
1081de5c  2100000a  beq 0x1081dee8
1081de60  863601eb  bl 0x1086b880
1081de64  000094e5  ldr r0, [r4,#0x0]
1081de68  020510e3  tst r0, #0x800000
1081de6c  0300000a  beq 0x1081de80
1081de70  0c0095e5  ldr r0, [r5,#0xc]
1081de74  1920d7e5  ldrb r2, [r7,#0x19]
1081de78  6610a0e3  mov r1, #0x66
1081de7c  ea3801eb  bl 0x1086c22c
1081de80  0700d4e5  ldrb r0, [r4,#0x7]
1081de84  020010e3  tst r0, #0x2
1081de88  0700000a  beq 0x1081deac
1081de8c  0000a0e3  mov r0, #0x0
1081de90  00008de5  str r0, [sp,#0x0]
1081de94  04008de5  str r0, [sp,#0x4]
1081de98  0b30d4e5  ldrb r3, [r4,#0xb]
1081de9c  0a20d4e5  ldrb r2, [r4,#0xa]
1081dea0  b810d4e1  ldrh r1, [r4,#0x8]
1081dea4  0500a0e1  cpy r0, r5
1081dea8  0c3201eb  bl 0x1086a6e0
1081deac  b420d4e1  ldrh r2, [r4,#0x4]
1081deb0  0c3084e2  add r3, r4, #0xc
1081deb4  0610a0e1  cpy r1, r6
1081deb8  0500a0e1  cpy r0, r5
1081debc  bf2401eb  bl 0x108671c0
1081dec0  000094e5  ldr r0, [r4,#0x0]
1081dec4  020510e3  tst r0, #0x800000
1081dec8  0300000a  beq 0x1081dedc
1081decc  0c0095e5  ldr r0, [r5,#0xc]
1081ded0  1920d7e5  ldrb r2, [r7,#0x19]
1081ded4  6710a0e3  mov r1, #0x67
1081ded8  d33801eb  bl 0x1086c22c
1081dedc  08d08de2  add sp, sp, #0x8
1081dee0  0100a0e3  mov r0, #0x1
1081dee4  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
1081dee8  00f020e3  nop
1081deec  633601eb  bl 0x1086b880
1081def0  08d08de2  add sp, sp, #0x8
1081def4  0000a0e3  mov r0, #0x0
1081def8  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
