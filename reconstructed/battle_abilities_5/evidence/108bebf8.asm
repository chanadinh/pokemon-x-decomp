108bebf8  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108bebfc  0240a0e1  cpy r4, r2
108bec00  0370a0e1  cpy r7, r3
108bec04  0180a0e1  cpy r8, r1
108bec08  0300a0e3  mov r0, #0x3
108bec0c  42b3feeb  bl 0x1086b91c
108bec10  ff6000e2  and r6, r0, #0xff
108bec14  040056e1  cmp r6, r4
108bec18  1e00000a  beq 0x108bec98
108bec1c  1600a0e3  mov r0, #0x16
108bec20  3db3feeb  bl 0x1086b91c
108bec24  0a0050e3  cmp r0, #0xa
108bec28  1a00001a  bne 0x108bec98
108bec2c  1200a0e3  mov r0, #0x12
108bec30  39b3feeb  bl 0x1086b91c
108bec34  7050ffe6  uxth r5, r0
108bec38  0530a0e1  cpy r3, r5
108bec3c  0420a0e1  cpy r2, r4
108bec40  0610a0e1  cpy r1, r6
108bec44  0800a0e1  cpy r0, r8
108bec48  8b32feeb  bl 0x1084b67c
108bec4c  000050e3  cmp r0, #0x0
108bec50  1000001a  bne 0x108bec98
108bec54  0500a0e1  cpy r0, r5
108bec58  b036feeb  bl 0x1084c720
108bec5c  000050e3  cmp r0, #0x0
108bec60  0c00001a  bne 0x108bec98
108bec64  010c45e2  sub r0, r5, #0x100
108bec68  fb0050e2  subs r0, r0, #0xfb
108bec6c  0900000a  beq 0x108bec98
108bec70  0400a0e3  mov r0, #0x4
108bec74  28b3feeb  bl 0x1086b91c
108bec78  ff5000e2  and r5, r0, #0xff
108bec7c  0410a0e1  cpy r1, r4
108bec80  0400a0e3  mov r0, #0x4
108bec84  129bfeeb  bl 0x108658d4
108bec88  000050e3  cmp r0, #0x0
108bec8c  04005511  cmpne r5, r4
108bec90  0100a013  movne r0, #0x1
108bec94  0000001a  bne 0x108bec9c
108bec98  0000a0e3  mov r0, #0x0
108bec9c  000087e5  str r0, [r7,#0x0]
108beca0  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
