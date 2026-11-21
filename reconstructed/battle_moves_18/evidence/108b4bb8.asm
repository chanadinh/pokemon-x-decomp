108b4bb8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b4bbc  0150a0e1  cpy r5, r1
108b4bc0  0240a0e1  cpy r4, r2
108b4bc4  0300a0e3  mov r0, #0x3
108b4bc8  53dbfeeb  bl 0x1086b91c
108b4bcc  040050e1  cmp r0, r4
108b4bd0  0f00001a  bne 0x108b4c14
108b4bd4  0500a0e1  cpy r0, r5
108b4bd8  9e5cfeeb  bl 0x1084be58
108b4bdc  010050e3  cmp r0, #0x1
108b4be0  30109f05  ldreq r1, [0x108b4c18]
108b4be4  0500000a  beq 0x108b4c00
108b4be8  020050e3  cmp r0, #0x2
108b4bec  03005013  cmpne r0, #0x3
108b4bf0  0500000a  beq 0x108b4c0c
108b4bf4  040050e3  cmp r0, #0x4
108b4bf8  021ba013  movne r1, #0x800
108b4bfc  0200000a  beq 0x108b4c0c
108b4c00  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b4c04  3600a0e3  mov r0, #0x36
108b4c08  31c3feea  b 0x108658d4
108b4c0c  011ba0e3  mov r1, #0x400
108b4c10  faffffea  b 0x108b4c00
108b4c14  7080bde8  ldmia sp!,{r4,r5,r6,pc}
