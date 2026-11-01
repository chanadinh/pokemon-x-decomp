108bf864  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bf868  0160a0e1  cpy r6, r1
108bf86c  0240a0e1  cpy r4, r2
108bf870  0350a0e1  cpy r5, r3
108bf874  0400a0e3  mov r0, #0x4
108bf878  27b0feeb  bl 0x1086b91c
108bf87c  040050e1  cmp r0, r4
108bf880  1000001a  bne 0x108bf8c8
108bf884  0600a0e1  cpy r0, r6
108bf888  7231feeb  bl 0x1084be58
108bf88c  010050e3  cmp r0, #0x1
108bf890  0c00001a  bne 0x108bf8c8
108bf894  1e00a0e3  mov r0, #0x1e
108bf898  1fb0feeb  bl 0x1086b91c
108bf89c  7040ffe6  uxth r4, r0
108bf8a0  0400a0e1  cpy r0, r4
108bf8a4  9631feeb  bl 0x1084bf04
108bf8a8  000050e3  cmp r0, #0x0
108bf8ac  0100001a  bne 0x108bf8b8
108bf8b0  0e0054e3  cmp r4, #0xe
108bf8b4  0300001a  bne 0x108bf8c8
108bf8b8  0110a0e3  mov r1, #0x1
108bf8bc  4200a0e3  mov r0, #0x42
108bf8c0  0398feeb  bl 0x108658d4
108bf8c4  000085e5  str r0, [r5,#0x0]
108bf8c8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
