108bf18c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bf190  0160a0e1  cpy r6, r1
108bf194  0240a0e1  cpy r4, r2
108bf198  0400a0e3  mov r0, #0x4
108bf19c  deb1feeb  bl 0x1086b91c
108bf1a0  040050e1  cmp r0, r4
108bf1a4  1e00001a  bne 0x108bf224
108bf1a8  2100a0e3  mov r0, #0x21
108bf1ac  dab1feeb  bl 0x1086b91c
108bf1b0  7050ffe6  uxth r5, r0
108bf1b4  0010a0e3  mov r1, #0x0
108bf1b8  2100a0e3  mov r0, #0x21
108bf1bc  c499feeb  bl 0x108658d4
108bf1c0  000055e3  cmp r5, #0x0
108bf1c4  1600000a  beq 0x108bf224
108bf1c8  0420a0e1  cpy r2, r4
108bf1cc  0610a0e3  mov r1, #0x6
108bf1d0  0600a0e1  cpy r0, r6
108bf1d4  68abfeeb  bl 0x10869f7c
108bf1d8  0040a0e1  cpy r4, r0
108bf1dc  000090e5  ldr r0, [r0,#0x0]
108bf1e0  020580e3  orr r0, r0, #0x800000
108bf1e4  000084e5  str r0, [r4,#0x0]
108bf1e8  0300a0e3  mov r0, #0x3
108bf1ec  cab1feeb  bl 0x1086b91c
108bf1f0  b450c4e1  strh r5, [r4,#0x4]
108bf1f4  0600c4e5  strb r0, [r4,#0x6]
108bf1f8  28209fe5  ldr r2, [0x108bf228]
108bf1fc  0210a0e3  mov r1, #0x2
108bf200  0c0084e2  add r0, r4, #0xc
108bf204  3238feeb  bl 0x1084d2d4
108bf208  0610d4e5  ldrb r1, [r4,#0x6]
108bf20c  0c0084e2  add r0, r4, #0xc
108bf210  2138feeb  bl 0x1084d29c
108bf214  0410a0e1  cpy r1, r4
108bf218  0600a0e1  cpy r0, r6
108bf21c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bf220  48a2feea  b 0x10867b48
108bf224  7080bde8  ldmia sp!,{r4,r5,r6,pc}
