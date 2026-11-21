1081ca98  042090e5  ldr r2, [r0,#0x4]
1081ca9c  102092e5  ldr r2, [r2,#0x10]
1081caa0  0020d2e5  ldrb r2, [r2,#0x0]
1081caa4  000052e3  cmp r2, #0x0
1081caa8  01005213  cmpne r2, #0x1
1081caac  0000a013  movne r0, #0x0
1081cab0  0c00001a  bne 0x1081cae8
1081cab4  000090e5  ldr r0, [r0,#0x0]
1081cab8  100090e5  ldr r0, [r0,#0x10]
1081cabc  1c28d0e5  ldrb r2, [r0,#0x81c]
1081cac0  080012e3  tst r2, #0x8
1081cac4  0600001a  bne 0x1081cae4
1081cac8  04c890e5  ldr r12, [r0,#0x804]
1081cacc  18209fe5  ldr r2, [0x1081caec]
1081cad0  0c1081e0  add r1, r1, r12
1081cad4  020051e1  cmp r1, r2
1081cad8  041880e5  str r1, [r0,#0x804]
1081cadc  0210a081  cpyhi r1, r2
1081cae0  041880e5  str r1, [r0,#0x804]
1081cae4  0100a0e3  mov r0, #0x1
1081cae8  1eff2fe1  bx lr
