108bad6c  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108bad70  0260a0e1  cpy r6, r2
108bad74  0170a0e1  cpy r7, r1
108bad78  0350a0e1  cpy r5, r3
108bad7c  0200a0e3  mov r0, #0x2
108bad80  e5c2feeb  bl 0x1086b91c
108bad84  060050e1  cmp r0, r6
108bad88  2e00001a  bne 0x108bae48
108bad8c  0e00a0e3  mov r0, #0xe
108bad90  e1c2feeb  bl 0x1086b91c
108bad94  ff4000e2  and r4, r0, #0xff
108bad98  0a0054e3  cmp r4, #0xa
108bad9c  0900001a  bne 0x108badc8
108bada0  0610a0e1  cpy r1, r6
108bada4  0700a0e1  cpy r0, r7
108bada8  7247feeb  bl 0x1084cb78
108badac  0040a0e1  cpy r4, r0
108badb0  0700a0e1  cpy r0, r7
108badb4  5644feeb  bl 0x1084bf14
108badb8  0020a0e3  mov r2, #0x0
108badbc  0410a0e1  cpy r1, r4
108badc0  15ccfeeb  bl 0x1086de1c
108badc4  0040a0e1  cpy r4, r0
108badc8  0520a0e1  cpy r2, r5
108badcc  0410a0e1  cpy r1, r4
108badd0  0700a0e1  cpy r0, r7
108badd4  9143feeb  bl 0x1084bc20
108badd8  000050e3  cmp r0, #0x0
108baddc  00f020e3  nop
108bade0  1800000a  beq 0x108bae48
108bade4  0010d5e5  ldrb r1, [r5,#0x0]
108bade8  0700a0e1  cpy r0, r7
108badec  ab65feeb  bl 0x108544a0
108badf0  00f020e3  nop
108badf4  00f020e3  nop
108badf8  7f46feeb  bl 0x1084c7fc
108badfc  0050b0e1  movs r5, r0
108bae00  00f020e3  nop
108bae04  0b00000a  beq 0x108bae38
108bae08  0610a0e3  mov r1, #0x6
108bae0c  a215fdeb  bl 0x1080049c
108bae10  000050e3  cmp r0, #0x0
108bae14  00f020e3  nop
108bae18  0600000a  beq 0x108bae38
108bae1c  0510a0e1  cpy r1, r5
108bae20  1200a0e3  mov r0, #0x12
108bae24  aaaafeeb  bl 0x108658d4
108bae28  0410a0e1  cpy r1, r4
108bae2c  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108bae30  0d00a0e3  mov r0, #0xd
108bae34  a6aafeea  b 0x108658d4
108bae38  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108bae3c  0110a0e3  mov r1, #0x1
108bae40  4200a0e3  mov r0, #0x42
108bae44  a2aafeea  b 0x108658d4
108bae48  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
