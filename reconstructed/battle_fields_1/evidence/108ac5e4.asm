108ac5e4  70402de9  stmdb sp!,{r4,r5,r6,lr}
108ac5e8  0250a0e1  cpy r5, r2
108ac5ec  0400a0e3  mov r0, #0x4
108ac5f0  c9fcfeeb  bl 0x1086b91c
108ac5f4  ff4000e2  and r4, r0, #0xff
108ac5f8  0400a0e1  cpy r0, r4
108ac5fc  6f7efeeb  bl 0x1084bfc0
108ac600  050050e1  cmp r0, r5
108ac604  1e00001a  bne 0x108ac684
108ac608  0300a0e3  mov r0, #0x3
108ac60c  c2fcfeeb  bl 0x1086b91c
108ac610  040050e1  cmp r0, r4
108ac614  1a00000a  beq 0x108ac684
108ac618  1200a0e3  mov r0, #0x12
108ac61c  befcfeeb  bl 0x1086b91c
108ac620  7050ffe6  uxth r5, r0
108ac624  1900a0e3  mov r0, #0x19
108ac628  bbfcfeeb  bl 0x1086b91c
108ac62c  010050e3  cmp r0, #0x1
108ac630  130000ba  blt 0x108ac684
108ac634  0310a0e3  mov r1, #0x3
108ac638  0500a0e1  cpy r0, r5
108ac63c  964ffdeb  bl 0x1080049c
108ac640  000050e3  cmp r0, #0x0
108ac644  0e00000a  beq 0x108ac684
108ac648  0110a0e3  mov r1, #0x1
108ac64c  4100a0e3  mov r0, #0x41
108ac650  9fe4feeb  bl 0x108658d4
108ac654  000050e3  cmp r0, #0x0
108ac658  0900000a  beq 0x108ac684
108ac65c  4000a0e3  mov r0, #0x40
108ac660  adfcfeeb  bl 0x1086b91c
108ac664  0050a0e1  cpy r5, r0
108ac668  322ea0e3  mov r2, #0x320
108ac66c  0210a0e3  mov r1, #0x2
108ac670  1783feeb  bl 0x1084d2d4
108ac674  0410a0e1  cpy r1, r4
108ac678  0500a0e1  cpy r0, r5
108ac67c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108ac680  0583feea  b 0x1084d29c
108ac684  7080bde8  ldmia sp!,{r4,r5,r6,pc}
