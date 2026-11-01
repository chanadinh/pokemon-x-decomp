108bd200  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bd204  0150a0e1  cpy r5, r1
108bd208  0240a0e1  cpy r4, r2
108bd20c  0300a0e3  mov r0, #0x3
108bd210  c1b9feeb  bl 0x1086b91c
108bd214  040050e1  cmp r0, r4
108bd218  0e00001a  bne 0x108bd258
108bd21c  1200a0e3  mov r0, #0x12
108bd220  bdb9feeb  bl 0x1086b91c
108bd224  7000ffe6  uxth r0, r0
108bd228  763dfeeb  bl 0x1084c808
108bd22c  000050e3  cmp r0, #0x0
108bd230  0800001a  bne 0x108bd258
108bd234  0500a0e1  cpy r0, r5
108bd238  0410a0e1  cpy r1, r4
108bd23c  a739feeb  bl 0x1084b8e0
108bd240  000050e3  cmp r0, #0x0
108bd244  0300000a  beq 0x108bd258
108bd248  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bd24c  3200a0e3  mov r0, #0x32
108bd250  04109fe5  ldr r1, [0x108bd25c]
108bd254  0a3ffeea  b 0x1084ce84
108bd258  7080bde8  ldmia sp!,{r4,r5,r6,pc}
