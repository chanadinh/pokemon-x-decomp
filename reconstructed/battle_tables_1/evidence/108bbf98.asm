108bbf98  010051e3  cmp r1, #0x1
108bbf9c  0b00000a  beq 0x108bbfd0
108bbfa0  020051e3  cmp r1, #0x2
108bbfa4  0320a0e3  mov r2, #0x3
108bbfa8  00208005  streq r2, [r0,#0x0]
108bbfac  2c009f05  ldreq r0, [0x108bbfe0]
108bbfb0  0500000a  beq 0x108bbfcc
108bbfb4  030051e3  cmp r1, #0x3
108bbfb8  0110a013  movne r1, #0x1
108bbfbc  00108015  strne r1, [r0,#0x0]
108bbfc0  00208005  streq r2, [r0,#0x0]
108bbfc4  18009f15  ldrne r0, [0x108bbfe4]
108bbfc8  18009f05  ldreq r0, [0x108bbfe8]
108bbfcc  1eff2fe1  bx lr
108bbfd0  0210a0e3  mov r1, #0x2
108bbfd4  001080e5  str r1, [r0,#0x0]
108bbfd8  0c009fe5  ldr r0, [0x108bbfec]
108bbfdc  1eff2fe1  bx lr
