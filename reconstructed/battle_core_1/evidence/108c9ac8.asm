108c9ac8  f0002de9  stmdb sp!,{r4,r5,r6,r7}
108c9acc  00c0a0e1  cpy r12, r0
108c9ad0  003090e5  ldr r3, [r0,#0x0]
108c9ad4  48709fe5  ldr r7, [0x108c9b24]
108c9ad8  0000a0e3  mov r0, #0x0
108c9adc  0340a0e1  cpy r4, r3
108c9ae0  013043e2  sub r3, r3, #0x1
108c9ae4  000053e3  cmp r3, #0x0
108c9ae8  0730a0b1  cpylt r3, r7
108c9aec  030054e1  cmp r4, r3
108c9af0  0900000a  beq 0x108c9b1c
108c9af4  83518ce0  add r5, r12, r3, lsl #0x3
108c9af8  b860d5e1  ldrh r6, [r5,#0x8]
108c9afc  000056e3  cmp r6, #0x0
108c9b00  0500000a  beq 0x108c9b1c
108c9b04  010056e1  cmp r6, r1
108c9b08  f4ffff1a  bne 0x108c9ae0
108c9b0c  045095e5  ldr r5, [r5,#0x4]
108c9b10  020055e1  cmp r5, r2
108c9b14  01008002  addeq r0, r0, #0x1
108c9b18  f0ffffea  b 0x108c9ae0
108c9b1c  f000bde8  ldmia sp!,{r4,r5,r6,r7}
108c9b20  1eff2fe1  bx lr
