0014ffd4  812082e0  add r2,r2,r1, lsl #0x1
0014ffd8  232182e0  add r2,r2,r3, lsr #0x2
0014ffdc  1c109fe5  ldr r1,[0x150000]
0014ffe0  920000e0  mul r0,r2,r0
0014ffe4  00c09de5  ldr r12,[sp,#0x0]
0014ffe8  0220a0e3  mov r2,#0x2
0014ffec  911080e0  umull r1,r0,r1,r0
0014fff0  0c10a0e1  cpy r1,r12
0014fff4  050080e2  add r0,r0,#0x5
0014fff8  7000ffe6  uxth r0,r0
0014fffc  7e6200ea  b 0x001689fc
