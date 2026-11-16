108beca4  ff5f2de9  stmdb sp!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108beca8  0300a0e3  mov r0, #0x3
108becac  0340a0e1  cpy r4, r3
108becb0  02a0a0e1  cpy r10, r2
108becb4  01b0a0e1  cpy r11, r1
108becb8  17b3feeb  bl 0x1086b91c
108becbc  0a0050e1  cmp r0, r10
108becc0  8e00001a  bne 0x108bef00
108becc4  0b00a0e1  cpy r0, r11
108becc8  0a10a0e1  cpy r1, r10
108beccc  f355feeb  bl 0x108544a0
108becd0  b201d0e1  ldrh r0, [r0,#0x12]
108becd4  000050e3  cmp r0, #0x0
108becd8  00009405  ldreq r0, [r4,#0x0]
108becdc  00005003  cmpeq r0, #0x0
108bece0  8600001a  bne 0x108bef00
108bece4  4e00a0e3  mov r0, #0x4e
108bece8  0bb3feeb  bl 0x1086b91c
108becec  000050e3  cmp r0, #0x0
108becf0  8200001a  bne 0x108bef00
108becf4  0500a0e3  mov r0, #0x5
108becf8  07b3feeb  bl 0x1086b91c
108becfc  005050e2  subs r5, r0, #0x0
108bed00  0060a0e3  mov r6, #0x0
108bed04  0600009a  bls 0x108bed24
108bed08  060086e2  add r0, r6, #0x6
108bed0c  ff0000e2  and r0, r0, #0xff
108bed10  01b3feeb  bl 0x1086b91c
108bed14  0600c4e7  strb r0, [r4,r6]
108bed18  016086e2  add r6, r6, #0x1
108bed1c  060055e1  cmp r5, r6
108bed20  f8ffff8a  bhi 0x108bed08
108bed24  060055e3  cmp r5, #0x6
108bed28  0180a0e3  mov r8, #0x1
108bed2c  0600003a  bcc 0x108bed4c
108bed30  0020a0e3  mov r2, #0x0
108bed34  0210a0e1  cpy r1, r2
108bed38  0200a0e1  cpy r0, r2
108bed3c  9405fdeb  bl 0x10800394
108bed40  00f020e3  nop
108bed44  00f020e3  nop
108bed48  320000ea  b 0x108bee18
108bed4c  b4919fe5  ldr r9, [0x108bef08]
108bed50  000055e3  cmp r5, #0x0
108bed54  0060a0e3  mov r6, #0x0
108bed58  1600009a  bls 0x108bedb8
108bed5c  0610d4e7  ldrb r1, [r4,r6]
108bed60  0b00a0e1  cpy r0, r11
108bed64  cd55feeb  bl 0x108544a0
108bed68  0010a0e1  cpy r1, r0
108bed6c  0120a0e3  mov r2, #0x1
108bed70  0b00a0e1  cpy r0, r11
108bed74  4d79fdeb  bl 0x1081d2b0
108bed78  867089e0  add r7, r9, r6, lsl #0x1
108bed7c  000058e3  cmp r8, #0x0
108bed80  b000c7e1  strh r0, [r7,#0x0]
108bed84  0800000a  beq 0x108bedac
108bed88  0610d4e7  ldrb r1, [r4,r6]
108bed8c  0a00a0e1  cpy r0, r10
108bed90  0436feeb  bl 0x1084c5a8
108bed94  000050e3  cmp r0, #0x0
108bed98  00f020e3  nop
108bed9c  0200000a  beq 0x108bedac
108beda0  b000d7e1  ldrh r0, [r7,#0x0]
108beda4  050080e2  add r0, r0, #0x5
108beda8  b000c7e1  strh r0, [r7,#0x0]
108bedac  016086e2  add r6, r6, #0x1
108bedb0  050056e1  cmp r6, r5
108bedb4  e8ffff3a  bcc 0x108bed5c
108bedb8  000055e3  cmp r5, #0x0
108bedbc  0000a0e3  mov r0, #0x0
108bedc0  1400009a  bls 0x108bee18
108bedc4  011080e2  add r1, r0, #0x1
108bedc8  050051e1  cmp r1, r5
108bedcc  80308930  addcc r3, r9, r0, lsl #0x1
108bedd0  0d00002a  bcs 0x108bee0c
108bedd4  81c089e0  add r12, r9, r1, lsl #0x1
108bedd8  b020d3e1  ldrh r2, [r3,#0x0]
108beddc  b060dce1  ldrh r6, [r12,#0x0]
108bede0  020056e1  cmp r6, r2
108bede4  0500002a  bcs 0x108bee00
108bede8  b060c3e1  strh r6, [r3,#0x0]
108bedec  b020cce1  strh r2, [r12,#0x0]
108bedf0  0160d4e7  ldrb r6, [r4,r1]
108bedf4  0020d4e7  ldrb r2, [r4,r0]
108bedf8  0060c4e7  strb r6, [r4,r0]
108bedfc  0120c4e7  strb r2, [r4,r1]
108bee00  011081e2  add r1, r1, #0x1
108bee04  050051e1  cmp r1, r5
108bee08  f1ffff3a  bcc 0x108bedd4
108bee0c  010080e2  add r0, r0, #0x1
108bee10  050050e1  cmp r0, r5
108bee14  eaffff3a  bcc 0x108bedc4
108bee18  000055e3  cmp r5, #0x0
108bee1c  0060a0e3  mov r6, #0x0
108bee20  1200009a  bls 0x108bee70
108bee24  0610d4e7  ldrb r1, [r4,r6]
108bee28  0b00a0e1  cpy r0, r11
108bee2c  9b55feeb  bl 0x108544a0
108bee30  0070a0e1  cpy r7, r0
108bee34  00f020e3  nop
108bee38  b201d0e1  ldrh r0, [r0,#0x12]
108bee3c  000050e3  cmp r0, #0x0
108bee40  00f020e3  nop
108bee44  0600000a  beq 0x108bee64
108bee48  0620d4e7  ldrb r2, [r4,r6]
108bee4c  0a10a0e1  cpy r1, r10
108bee50  0b00a0e1  cpy r0, r11
108bee54  4033feeb  bl 0x1084bb5c
108bee58  000050e3  cmp r0, #0x0
108bee5c  00f020e3  nop
108bee60  0600000a  beq 0x108bee80
108bee64  016086e2  add r6, r6, #0x1
108bee68  050056e1  cmp r6, r5
108bee6c  ecffff3a  bcc 0x108bee24
108bee70  00009de5  ldr r0, [sp,#0x0]
108bee74  10d08de2  add sp, sp, #0x10
108bee78  f05fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108bee7c  c6ecfeea  b 0x1087a19c
108bee80  0a20a0e1  cpy r2, r10
108bee84  2510a0e3  mov r1, #0x25
108bee88  0b00a0e1  cpy r0, r11
108bee8c  3aacfeeb  bl 0x10869f7c
108bee90  0050a0e1  cpy r5, r0
108bee94  000090e5  ldr r0, [r0,#0x0]
108bee98  6c209fe5  ldr r2, [0x108bef0c]
108bee9c  0210a0e3  mov r1, #0x2
108beea0  020580e3  orr r0, r0, #0x800000
108beea4  000085e5  str r0, [r5,#0x0]
108beea8  0600d4e7  ldrb r0, [r4,r6]
108beeac  0400c5e5  strb r0, [r5,#0x4]
108beeb0  080085e2  add r0, r5, #0x8
108beeb4  0639feeb  bl 0x1084d2d4
108beeb8  0a10a0e1  cpy r1, r10
108beebc  080085e2  add r0, r5, #0x8
108beec0  f538feeb  bl 0x1084d29c
108beec4  0610d4e7  ldrb r1, [r4,r6]
108beec8  080085e2  add r0, r5, #0x8
108beecc  f238feeb  bl 0x1084d29c
108beed0  0700a0e1  cpy r0, r7
108beed4  00f020e3  nop
108beed8  b201d0e1  ldrh r0, [r0,#0x12]
108beedc  0010a0e1  cpy r1, r0
108beee0  080085e2  add r0, r5, #0x8
108beee4  ec38feeb  bl 0x1084d29c
108beee8  0510a0e1  cpy r1, r5
108beeec  0b00a0e1  cpy r0, r11
108beef0  14a3feeb  bl 0x10867b48
108beef4  00f020e3  nop
108beef8  00f020e3  nop
108beefc  dbffffea  b 0x108bee70
108bef00  10d08de2  add sp, sp, #0x10
108bef04  f09fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,pc}
