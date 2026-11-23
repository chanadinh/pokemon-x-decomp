1081cd58  38402de9  stmdb sp!,{r3,r4,r5,lr}
1081cd5c  03c0a0e1  cpy r12, r3
1081cd60  88319fe5  ldr r3, [0x1081cef0]
1081cd64  1910d1e5  ldrb r1, [r1,#0x19]
1081cd68  010052e3  cmp r2, #0x1
1081cd6c  014a80e2  add r4, r0, #0x1000
1081cd70  0200000a  beq 0x1081cd80
1081cd74  020052e3  cmp r2, #0x2
1081cd78  1200001a  bne 0x1081cdc8
1081cd7c  540000ea  b 0x1081ced4
1081cd80  05005ce3  cmp r12, #0x5
1081cd84  1300000a  beq 0x1081cdd8
1081cd88  080000ca  bgt 0x1081cdb0
1081cd8c  01005ce3  cmp r12, #0x1
1081cd90  2200000a  beq 0x1081ce20
1081cd94  02005ce3  cmp r12, #0x2
1081cd98  2900000a  beq 0x1081ce44
1081cd9c  03005ce3  cmp r12, #0x3
1081cda0  3000000a  beq 0x1081ce68
1081cda4  04005ce3  cmp r12, #0x4
1081cda8  0600001a  bne 0x1081cdc8
1081cdac  120000ea  b 0x1081cdfc
1081cdb0  06005ce3  cmp r12, #0x6
1081cdb4  3400000a  beq 0x1081ce8c
1081cdb8  0f005ce3  cmp r12, #0xf
1081cdbc  3b00000a  beq 0x1081ceb0
1081cdc0  14005ce3  cmp r12, #0x14
1081cdc4  4100000a  beq 0x1081ced0
1081cdc8  0c0090e5  ldr r0, [r0,#0xc]
1081cdcc  4a20a0e3  mov r2, #0x4a
1081cdd0  6910a0e3  mov r1, #0x69
1081cdd4  430000ea  b 0x1081cee8
1081cdd8  00308de5  str r3, [sp,#0x0]
1081cddc  0130a0e1  cpy r3, r1
1081cde0  0c0090e5  ldr r0, [r0,#0xc]
1081cde4  f920a0e3  mov r2, #0xf9
1081cde8  6a10a0e3  mov r1, #0x6a
1081cdec  fb3901eb  bl 0x1086b5e0
1081cdf0  00f020e3  nop
1081cdf4  00f020e3  nop
1081cdf8  310000ea  b 0x1081cec4
1081cdfc  00308de5  str r3, [sp,#0x0]
1081ce00  0130a0e1  cpy r3, r1
1081ce04  0c0090e5  ldr r0, [r0,#0xc]
1081ce08  e4209fe5  ldr r2, [0x1081cef4]
1081ce0c  6a10a0e3  mov r1, #0x6a
1081ce10  f23901eb  bl 0x1086b5e0
1081ce14  00f020e3  nop
1081ce18  00f020e3  nop
1081ce1c  280000ea  b 0x1081cec4
1081ce20  00308de5  str r3, [sp,#0x0]
1081ce24  0130a0e1  cpy r3, r1
1081ce28  0c0090e5  ldr r0, [r0,#0xc]
1081ce2c  c4209fe5  ldr r2, [0x1081cef8]
1081ce30  6a10a0e3  mov r1, #0x6a
1081ce34  e93901eb  bl 0x1086b5e0
1081ce38  00f020e3  nop
1081ce3c  00f020e3  nop
1081ce40  1f0000ea  b 0x1081cec4
1081ce44  00308de5  str r3, [sp,#0x0]
1081ce48  0130a0e1  cpy r3, r1
1081ce4c  0c0090e5  ldr r0, [r0,#0xc]
1081ce50  a4209fe5  ldr r2, [0x1081cefc]
1081ce54  6a10a0e3  mov r1, #0x6a
1081ce58  e03901eb  bl 0x1086b5e0
1081ce5c  00f020e3  nop
1081ce60  00f020e3  nop
1081ce64  160000ea  b 0x1081cec4
1081ce68  00308de5  str r3, [sp,#0x0]
1081ce6c  0130a0e1  cpy r3, r1
1081ce70  0c0090e5  ldr r0, [r0,#0xc]
1081ce74  84209fe5  ldr r2, [0x1081cf00]
1081ce78  6a10a0e3  mov r1, #0x6a
1081ce7c  d73901eb  bl 0x1086b5e0
1081ce80  00f020e3  nop
1081ce84  00f020e3  nop
1081ce88  0d0000ea  b 0x1081cec4
1081ce8c  00308de5  str r3, [sp,#0x0]
1081ce90  0130a0e1  cpy r3, r1
1081ce94  0c0090e5  ldr r0, [r0,#0xc]
1081ce98  64209fe5  ldr r2, [0x1081cf04]
1081ce9c  6a10a0e3  mov r1, #0x6a
1081cea0  ce3901eb  bl 0x1086b5e0
1081cea4  00f020e3  nop
1081cea8  00f020e3  nop
1081ceac  040000ea  b 0x1081cec4
1081ceb0  00308de5  str r3, [sp,#0x0]
1081ceb4  0c0090e5  ldr r0, [r0,#0xc]
1081ceb8  0130a0e1  cpy r3, r1
1081cebc  1820a0e3  mov r2, #0x18
1081cec0  070000ea  b 0x1081cee4
1081cec4  9300d4e5  ldrb r0, [r4,#0x93]
1081cec8  100080e3  orr r0, r0, #0x10
1081cecc  9300c4e5  strb r0, [r4,#0x93]
1081ced0  3880bde8  ldmia sp!,{r3,r4,r5,pc}
1081ced4  00308de5  str r3, [sp,#0x0]
1081ced8  0c0090e5  ldr r0, [r0,#0xc]
1081cedc  0130a0e1  cpy r3, r1
1081cee0  d220a0e3  mov r2, #0xd2
1081cee4  6a10a0e3  mov r1, #0x6a
1081cee8  bc3901eb  bl 0x1086b5e0
1081ceec  f4ffffea  b 0x1081cec4
