108abdc0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108abdc4  0060a0e1  cpy r6, r0
108abdc8  000093e5  ldr r0, [r3,#0x0]
108abdcc  0140a0e1  cpy r4, r1
108abdd0  0250a0e1  cpy r5, r2
108abdd4  000050e3  cmp r0, #0x0
108abdd8  1300001a  bne 0x108abe2c
108abddc  0210a0e1  cpy r1, r2
108abde0  0400a0e1  cpy r0, r4
108abde4  ada1feeb  bl 0x108544a0
108abde8  0f10a0e3  mov r1, #0xf
108abdec  25abfeeb  bl 0x10856a88
108abdf0  000050e3  cmp r0, #0x0
108abdf4  0c00001a  bne 0x108abe2c
108abdf8  0220a0e3  mov r2, #0x2
108abdfc  0510a0e1  cpy r1, r5
108abe00  0400a0e1  cpy r0, r4
108abe04  4982feeb  bl 0x1084c730
108abe08  000050e3  cmp r0, #0x0
108abe0c  0600000a  beq 0x108abe2c
108abe10  0600a0e1  cpy r0, r6
108abe14  d482feeb  bl 0x1084c96c
108abe18  0520a0e1  cpy r2, r5
108abe1c  0400a0e1  cpy r0, r4
108abe20  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108abe24  0010a0e3  mov r1, #0x0
108abe28  c682feea  b 0x1084c948
108abe2c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
