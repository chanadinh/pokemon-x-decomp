1084beac  14209fe5  ldr r2, [0x1084bec8]
1084beb0  7e3fa0e3  mov r3, #0x1f8
1084beb4  800103e0  and r0, r3, r0, lsl #0x3
1084beb8  020080e3  orr r0, r0, #0x2
1084bebc  0200c0e1  bic r0, r0, r2
1084bec0  810480e1  orr r0, r0, r1, lsl #0x9
1084bec4  1eff2fe1  bx lr
