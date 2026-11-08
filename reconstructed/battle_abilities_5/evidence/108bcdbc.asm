108bcdbc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bcdc0  0160a0e1  cpy r6, r1
108bcdc4  0250a0e1  cpy r5, r2
108bcdc8  0400a0e3  mov r0, #0x4
108bcdcc  d2bafeeb  bl 0x1086b91c
108bcdd0  050050e1  cmp r0, r5
108bcdd4  2200001a  bne 0x108bce64
108bcdd8  1200a0e3  mov r0, #0x12
108bcddc  cebafeeb  bl 0x1086b91c
108bcde0  7000ffe6  uxth r0, r0
108bcde4  369effeb  bl 0x108a46c4
108bcde8  000050e3  cmp r0, #0x0
108bcdec  1c00000a  beq 0x108bce64
108bcdf0  0110a0e3  mov r1, #0x1
108bcdf4  4100a0e3  mov r0, #0x41
108bcdf8  b5a2feeb  bl 0x108658d4
108bcdfc  000050e3  cmp r0, #0x0
108bce00  1700000a  beq 0x108bce64
108bce04  0520a0e1  cpy r2, r5
108bce08  0110a0e3  mov r1, #0x1
108bce0c  0600a0e1  cpy r0, r6
108bce10  cc3efeeb  bl 0x1084c948
108bce14  0520a0e1  cpy r2, r5
108bce18  0310a0e3  mov r1, #0x3
108bce1c  0600a0e1  cpy r0, r6
108bce20  55b4feeb  bl 0x10869f7c
108bce24  0040a0e1  cpy r4, r0
108bce28  d220a0e3  mov r2, #0xd2
108bce2c  0210a0e3  mov r1, #0x2
108bce30  040080e2  add r0, r0, #0x4
108bce34  2641feeb  bl 0x1084d2d4
108bce38  0510a0e1  cpy r1, r5
108bce3c  040084e2  add r0, r4, #0x4
108bce40  1541feeb  bl 0x1084d29c
108bce44  0410a0e1  cpy r1, r4
108bce48  0600a0e1  cpy r0, r6
108bce4c  3dabfeeb  bl 0x10867b48
108bce50  0520a0e1  cpy r2, r5
108bce54  0600a0e1  cpy r0, r6
108bce58  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bce5c  0210a0e3  mov r1, #0x2
108bce60  b83efeea  b 0x1084c948
108bce64  7080bde8  ldmia sp!,{r4,r5,r6,pc}
