108beb00  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108beb04  0250a0e1  cpy r5, r2
108beb08  0170a0e1  cpy r7, r1
108beb0c  0200a0e3  mov r0, #0x2
108beb10  81b3feeb  bl 0x1086b91c
108beb14  050050e1  cmp r0, r5
108beb18  3400001a  bne 0x108bebf0
108beb1c  0510a0e1  cpy r1, r5
108beb20  0700a0e1  cpy r0, r7
108beb24  5d56feeb  bl 0x108544a0
108beb28  0040a0e1  cpy r4, r0
108beb2c  b401d0e1  ldrh r0, [r0,#0x14]
108beb30  0060b0e1  movs r6, r0
108beb34  2d00000a  beq 0x108bebf0
108beb38  1907fdeb  bl 0x108007a4
108beb3c  000050e3  cmp r0, #0x0
108beb40  2a00000a  beq 0x108bebf0
108beb44  0400a0e1  cpy r0, r4
108beb48  b201d0e1  ldrh r0, [r0,#0x12]
108beb4c  000050e3  cmp r0, #0x0
108beb50  2600001a  bne 0x108bebf0
108beb54  0700a0e1  cpy r0, r7
108beb58  be34feeb  bl 0x1084be58
108beb5c  010050e3  cmp r0, #0x1
108beb60  0800000a  beq 0x108beb88
108beb64  3200a0e3  mov r0, #0x32
108beb68  6235feeb  bl 0x1084c0f8
108beb6c  000050e3  cmp r0, #0x0
108beb70  0400001a  bne 0x108beb88
108beb74  0110a0e3  mov r1, #0x1
108beb78  0700a0e1  cpy r0, r7
108beb7c  5a35feeb  bl 0x1084c0ec
108beb80  000050e3  cmp r0, #0x0
108beb84  1900000a  beq 0x108bebf0
108beb88  0520a0e1  cpy r2, r5
108beb8c  2110a0e3  mov r1, #0x21
108beb90  0700a0e1  cpy r0, r7
108beb94  f8acfeeb  bl 0x10869f7c
108beb98  0040a0e1  cpy r4, r0
108beb9c  000090e5  ldr r0, [r0,#0x0]
108beba0  b460c4e1  strh r6, [r4,#0x4]
108beba4  0110a0e3  mov r1, #0x1
108beba8  0650c4e5  strb r5, [r4,#0x6]
108bebac  0710c4e5  strb r1, [r4,#0x7]
108bebb0  020580e3  orr r0, r0, #0x800000
108bebb4  000084e5  str r0, [r4,#0x0]
108bebb8  34209fe5  ldr r2, [0x108bebf4]
108bebbc  0210a0e3  mov r1, #0x2
108bebc0  0c0084e2  add r0, r4, #0xc
108bebc4  c239feeb  bl 0x1084d2d4
108bebc8  0510a0e1  cpy r1, r5
108bebcc  0c0084e2  add r0, r4, #0xc
108bebd0  b139feeb  bl 0x1084d29c
108bebd4  0610a0e1  cpy r1, r6
108bebd8  0c0084e2  add r0, r4, #0xc
108bebdc  ae39feeb  bl 0x1084d29c
108bebe0  0410a0e1  cpy r1, r4
108bebe4  0700a0e1  cpy r0, r7
108bebe8  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108bebec  d5a3feea  b 0x10867b48
108bebf0  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
