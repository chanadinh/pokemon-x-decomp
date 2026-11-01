108be18c  1600a0e3  mov r0, #0x16
108be190  10402de9  stmdb sp!,{r4,lr}
108be194  e0b5feeb  bl 0x1086b91c
108be198  100050e3  cmp r0, #0x10
108be19c  0c00001a  bne 0x108be1d4
108be1a0  0110a0e3  mov r1, #0x1
108be1a4  5800a0e3  mov r0, #0x58
108be1a8  c99dfeeb  bl 0x108658d4
108be1ac  000050e3  cmp r0, #0x0
108be1b0  0700000a  beq 0x108be1d4
108be1b4  5700a0e3  mov r0, #0x57
108be1b8  d7b5feeb  bl 0x1086b91c
108be1bc  000050e3  cmp r0, #0x0
108be1c0  10109f05  ldreq r1, [0x108be1d8]
108be1c4  1040bde8  ldmia sp!,{r4,lr}
108be1c8  031ba013  movne r1, #0xc00
108be1cc  3200a0e3  mov r0, #0x32
108be1d0  2b3bfeea  b 0x1084ce84
108be1d4  1080bde8  ldmia sp!,{r4,pc}
