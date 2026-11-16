108a8588  f8402de9  stmdb sp!,{r3,r4,r5,r6,r7,lr}
108a858c  0050a0e1  cpy r5, r0
108a8590  0160a0e1  cpy r6, r1
108a8594  0240a0e1  cpy r4, r2
108a8598  0370a0e1  cpy r7, r3
108a859c  0200a0e3  mov r0, #0x2
108a85a0  dd0cffeb  bl 0x1086b91c
108a85a4  040050e1  cmp r0, r4
108a85a8  0600001a  bne 0x108a85c8
108a85ac  0000a0e3  mov r0, #0x0
108a85b0  00008de5  str r0, [sp,#0x0]
108a85b4  0730a0e1  cpy r3, r7
108a85b8  0420a0e1  cpy r2, r4
108a85bc  0610a0e1  cpy r1, r6
108a85c0  0500a0e1  cpy r0, r5
108a85c4  140600eb  bl 0x108a9e1c
108a85c8  f880bde8  ldmia sp!,{r3,r4,r5,r6,r7,pc}
