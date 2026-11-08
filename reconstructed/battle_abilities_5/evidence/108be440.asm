108be440  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108be444  0240a0e1  cpy r4, r2
108be448  0370a0e1  cpy r7, r3
108be44c  0180a0e1  cpy r8, r1
108be450  0300a0e3  mov r0, #0x3
108be454  30b5feeb  bl 0x1086b91c
108be458  ff6000e2  and r6, r0, #0xff
108be45c  040056e1  cmp r6, r4
108be460  1e00000a  beq 0x108be4e0
108be464  1600a0e3  mov r0, #0x16
108be468  2bb5feeb  bl 0x1086b91c
108be46c  0c0050e3  cmp r0, #0xc
108be470  1a00001a  bne 0x108be4e0
108be474  1200a0e3  mov r0, #0x12
108be478  27b5feeb  bl 0x1086b91c
108be47c  7050ffe6  uxth r5, r0
108be480  0530a0e1  cpy r3, r5
108be484  0420a0e1  cpy r2, r4
108be488  0610a0e1  cpy r1, r6
108be48c  0800a0e1  cpy r0, r8
108be490  7934feeb  bl 0x1084b67c
108be494  000050e3  cmp r0, #0x0
108be498  1000001a  bne 0x108be4e0
108be49c  0500a0e1  cpy r0, r5
108be4a0  9e38feeb  bl 0x1084c720
108be4a4  000050e3  cmp r0, #0x0
108be4a8  0c00001a  bne 0x108be4e0
108be4ac  010c45e2  sub r0, r5, #0x100
108be4b0  fb0050e2  subs r0, r0, #0xfb
108be4b4  0900000a  beq 0x108be4e0
108be4b8  0400a0e3  mov r0, #0x4
108be4bc  16b5feeb  bl 0x1086b91c
108be4c0  ff5000e2  and r5, r0, #0xff
108be4c4  0410a0e1  cpy r1, r4
108be4c8  0400a0e3  mov r0, #0x4
108be4cc  009dfeeb  bl 0x108658d4
108be4d0  000050e3  cmp r0, #0x0
108be4d4  04005511  cmpne r5, r4
108be4d8  0100a013  movne r0, #0x1
108be4dc  0000001a  bne 0x108be4e4
108be4e0  0000a0e3  mov r0, #0x0
108be4e4  000087e5  str r0, [r7,#0x0]
108be4e8  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
