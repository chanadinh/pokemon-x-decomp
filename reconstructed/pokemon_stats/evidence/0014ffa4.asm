0014ffa4  812082e0  add r2,r2,r1, lsl #0x1
0014ffa8  232182e0  add r2,r2,r3, lsr #0x2
0014ffac  1c109fe5  ldr r1,[0x14ffd0]
0014ffb0  920000e0  mul r0,r2,r0
0014ffb4  00c09de5  ldr r12,[sp,#0x0]
0014ffb8  0120a0e3  mov r2,#0x1
0014ffbc  911080e0  umull r1,r0,r1,r0
0014ffc0  0c10a0e1  cpy r1,r12
0014ffc4  050080e2  add r0,r0,#0x5
0014ffc8  7000ffe6  uxth r0,r0
0014ffcc  8a6200ea  b 0x001689fc
