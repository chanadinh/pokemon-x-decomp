108bbb4c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bbb50  0150a0e1  cpy r5, r1
108bbb54  0240a0e1  cpy r4, r2
108bbb58  0100a0e1  cpy r0, r1
108bbb5c  ec40feeb  bl 0x1084bf14
108bbb60  000050e3  cmp r0, #0x0
108bbb64  0300000a  beq 0x108bbb78
108bbb68  0500a0e1  cpy r0, r5
108bbb6c  e840feeb  bl 0x1084bf14
108bbb70  030050e3  cmp r0, #0x3
108bbb74  0d00001a  bne 0x108bbbb0
108bbb78  0200a0e3  mov r0, #0x2
108bbb7c  66bffeeb  bl 0x1086b91c
108bbb80  040050e1  cmp r0, r4
108bbb84  00f020e3  nop
108bbb88  0800001a  bne 0x108bbbb0
108bbb8c  2300a0e3  mov r0, #0x23
108bbb90  61bffeeb  bl 0x1086b91c
108bbb94  000050e3  cmp r0, #0x0
108bbb98  00f020e3  nop
108bbb9c  0300001a  bne 0x108bbbb0
108bbba0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bbba4  1b10a0e3  mov r1, #0x1b
108bbba8  2300a0e3  mov r0, #0x23
108bbbac  48a7feea  b 0x108658d4
108bbbb0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
