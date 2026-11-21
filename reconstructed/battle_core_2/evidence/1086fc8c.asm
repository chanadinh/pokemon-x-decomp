1086fc8c  04402de5  str r4, [sp,#-0x4]!
1086fc90  18c0d0e5  ldrb r12, [r0,#0x18]
1086fc94  0030a0e1  cpy r3, r0
1086fc98  00005ce3  cmp r12, #0x0
1086fc9c  0020a003  moveq r2, #0x0
1086fca0  0900000a  beq 0x1086fccc
1086fca4  01200ce2  and r2, r12, #0x1
1086fca8  000052e3  cmp r2, #0x0
1086fcac  0000a0e3  mov r0, #0x0
1086fcb0  050000da  ble 0x1086fccc
1086fcb4  004193e7  ldr r4, [r3,r0,lsl #0x2]
1086fcb8  010054e1  cmp r4, r1
1086fcbc  0d00000a  beq 0x1086fcf8
1086fcc0  010080e2  add r0, r0, #0x1
1086fcc4  000052e1  cmp r2, r0
1086fcc8  f9ffffca  bgt 0x1086fcb4
1086fccc  0c0052e1  cmp r2, r12
1086fcd0  0200a0e1  cpy r0, r2
1086fcd4  0c0000aa  bge 0x1086fd0c
1086fcd8  002193e7  ldr r2, [r3,r0,lsl #0x2]
1086fcdc  010052e1  cmp r2, r1
1086fce0  0400000a  beq 0x1086fcf8
1086fce4  002183e0  add r2, r3, r0, lsl #0x2
1086fce8  042092e5  ldr r2, [r2,#0x4]
1086fcec  010052e1  cmp r2, r1
1086fcf0  0200001a  bne 0x1086fd00
1086fcf4  010080e2  add r0, r0, #0x1
1086fcf8  04409de4  ldr r4, [sp],#0x4
1086fcfc  1eff2fe1  bx lr
1086fd00  020080e2  add r0, r0, #0x2
1086fd04  00005ce1  cmp r12, r0
1086fd08  f2ffffca  bgt 0x1086fcd8
1086fd0c  04409de4  ldr r4, [sp],#0x4
1086fd10  0000e0e3  mvn r0, #0x0
1086fd14  1eff2fe1  bx lr
