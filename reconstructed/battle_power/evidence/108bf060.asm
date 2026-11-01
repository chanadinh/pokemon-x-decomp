108bf060  1600a0e3  mov r0, #0x16
108bf064  10402de9  stmdb sp!,{r4,lr}
108bf068  2bb2feeb  bl 0x1086b91c
108bf06c  110050e3  cmp r0, #0x11
108bf070  0c00001a  bne 0x108bf0a8
108bf074  0110a0e3  mov r1, #0x1
108bf078  5800a0e3  mov r0, #0x58
108bf07c  149afeeb  bl 0x108658d4
108bf080  000050e3  cmp r0, #0x0
108bf084  0700000a  beq 0x108bf0a8
108bf088  5700a0e3  mov r0, #0x57
108bf08c  22b2feeb  bl 0x1086b91c
108bf090  000050e3  cmp r0, #0x0
108bf094  10109f05  ldreq r1, [0x108bf0ac]
108bf098  1040bde8  ldmia sp!,{r4,lr}
108bf09c  031ba013  movne r1, #0xc00
108bf0a0  3200a0e3  mov r0, #0x32
108bf0a4  7637feea  b 0x1084ce84
108bf0a8  1080bde8  ldmia sp!,{r4,pc}
