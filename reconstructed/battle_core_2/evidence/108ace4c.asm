10876e04  181090e5  ldr r1, [r0,#0x18]
10876e08  020411e3  tst r1, #0x2000000
10876e0c  0214c113  bicne r1, r1, #0x2000000
10876e10  18108015  strne r1, [r0,#0x18]
10876e14  0100a013  movne r0, #0x1
10876e18  0000a003  moveq r0, #0x0
10876e1c  1eff2fe1  bx lr
108ace4c  18209fe5  ldr r2, [0x108ace6c]
108ace50  000280e0  add r0, r0, r0, lsl #0x4
108ace54  000282e0  add r0, r2, r0, lsl #0x4
108ace58  010290e7  ldr r0, [r0,r1,lsl #0x4]
108ace5c  000050e3  cmp r0, #0x0
108ace60  0000000a  beq 0x108ace68
108ace64  e627ffea  b 0x10876e04
108ace68  1eff2fe1  bx lr
