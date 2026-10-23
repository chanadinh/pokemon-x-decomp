0014ff74  812082e0  add r2,r2,r1, lsl #0x1
0014ff78  232182e0  add r2,r2,r3, lsr #0x2
0014ff7c  1c109fe5  ldr r1,[0x14ffa0]
0014ff80  920000e0  mul r0,r2,r0
0014ff84  00c09de5  ldr r12,[sp,#0x0]
0014ff88  0520a0e3  mov r2,#0x5
0014ff8c  911080e0  umull r1,r0,r1,r0
0014ff90  0c10a0e1  cpy r1,r12
0014ff94  050080e2  add r0,r0,#0x5
0014ff98  7000ffe6  uxth r0,r0
0014ff9c  966200ea  b 0x001689fc
