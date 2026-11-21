1084bfa0  010a80e2  add r0, r0, #0x1000
1084bfa4  830f80e2  add r0, r0, #0x20c
1084bfa8  04e02de5  str lr, [sp,#-0x4]!
1084bfac  c68000eb  bl 0x1086c2cc
1084bfb0  0a0050e3  cmp r0, #0xa
1084bfb4  0100a013  movne r0, #0x1
1084bfb8  0000a003  moveq r0, #0x0
1084bfbc  04f09de4  ldr pc, [sp],#0x4
