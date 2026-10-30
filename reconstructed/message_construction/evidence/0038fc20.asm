0038fc20  10402de9  stmdb sp!,{r4,lr}
0038fc24  081090e5  ldr r1, [r0,#0x8]
0038fc28  0040a0e1  cpy r4, r0
0038fc2c  40009fe5  ldr r0, [0x38fc74]
0038fc30  000051e3  cmp r1, #0x0
0038fc34  000084e5  str r0, [r4,#0x0]
0038fc38  0b00000a  beq 0x0038fc6c
0038fc3c  1c0094e5  ldr r0, [r4,#0x1c]
0038fc40  000050e3  cmp r0, #0x0
0038fc44  30aaf61b  blne 0x0013a50c
0038fc48  180094e5  ldr r0, [r4,#0x18]
0038fc4c  000050e3  cmp r0, #0x0
0038fc50  2daaf61b  blne 0x0013a50c
0038fc54  080094e5  ldr r0, [r4,#0x8]
0038fc58  000050e3  cmp r0, #0x0
0038fc5c  0200000a  beq 0x0038fc6c
0038fc60  001090e5  ldr r1, [r0,#0x0]
0038fc64  041091e5  ldr r1, [r1,#0x4]
0038fc68  31ff2fe1  blx r1
0038fc6c  0400a0e1  cpy r0, r4
0038fc70  1080bde8  ldmia sp!,{r4,pc}
