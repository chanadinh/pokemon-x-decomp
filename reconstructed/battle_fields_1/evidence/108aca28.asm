108aca28  70402de9  stmdb sp!,{r4,r5,r6,lr}
108aca2c  0150a0e1  cpy r5, r1
108aca30  0240a0e1  cpy r4, r2
108aca34  0400a0e3  mov r0, #0x4
108aca38  b7fbfeeb  bl 0x1086b91c
108aca3c  ff0000e2  and r0, r0, #0xff
108aca40  5e7dfeeb  bl 0x1084bfc0
108aca44  040050e1  cmp r0, r4
108aca48  1300001a  bne 0x108aca9c
108aca4c  1b00a0e3  mov r0, #0x1b
108aca50  b1fbfeeb  bl 0x1086b91c
108aca54  020050e3  cmp r0, #0x2
108aca58  0f00001a  bne 0x108aca9c
108aca5c  4600a0e3  mov r0, #0x46
108aca60  adfbfeeb  bl 0x1086b91c
108aca64  000050e3  cmp r0, #0x0
108aca68  0b00001a  bne 0x108aca9c
108aca6c  0500a0e1  cpy r0, r5
108aca70  277dfeeb  bl 0x1084bf14
108aca74  000050e3  cmp r0, #0x0
108aca78  0500000a  beq 0x108aca94
108aca7c  030050e3  cmp r0, #0x3
108aca80  18109f15  ldrne r1, [0x108acaa0]
108aca84  0200000a  beq 0x108aca94
108aca88  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108aca8c  3600a0e3  mov r0, #0x36
108aca90  fb80feea  b 0x1084ce84
108aca94  021ba0e3  mov r1, #0x800
108aca98  faffffea  b 0x108aca88
108aca9c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
