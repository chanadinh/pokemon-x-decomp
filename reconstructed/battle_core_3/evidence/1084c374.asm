1084c374  10402de9  stmdb sp!,{r4,lr}
1084c378  1c0090e5  ldr r0, [r0,#0x1c]
1084c37c  0b0051e3  cmp r1, #0xb
1084c380  0400002a  bcs 0x1084c398
1084c384  010180e0  add r0, r0, r1, lsl #0x2
1084c388  740290e5  ldr r0, [r0,#0x274]
1084c38c  000050e3  cmp r0, #0x0
1084c390  0100a013  movne r0, #0x1
1084c394  1080bde8  ldmia sp!,{r4,pc}
1084c398  0020a0e3  mov r2, #0x0
1084c39c  0210a0e1  cpy r1, r2
1084c3a0  0200a0e1  cpy r0, r2
1084c3a4  facffeeb  bl 0x10800394
1084c3a8  0000a0e3  mov r0, #0x0
1084c3ac  1080bde8  ldmia sp!,{r4,pc}
