004a16ac  10402de9  stmdb sp!,{r4,lr}
004a16b0  0c0090e5  ldr r0,[r0,#0xc]
004a16b4  49c6f2eb  bl 0x00152fe0
004a16b8  000050e3  cmp r0,#0x0
004a16bc  0100a003  moveq r0,#0x1
004a16c0  0000a013  movne r0,#0x0
004a16c4  1080bde8  ldmia sp!,{r4,pc}
