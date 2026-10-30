0038fbc4  10402de9  stmdb sp!,{r4,lr}
0038fbc8  4c209fe5  ldr r2, [0x38fc1c]
0038fbcc  081090e5  ldr r1, [r0,#0x8]
0038fbd0  0040a0e1  cpy r4, r0
0038fbd4  002080e5  str r2, [r0,#0x0]
0038fbd8  000051e3  cmp r1, #0x0
0038fbdc  0b00000a  beq 0x0038fc10
0038fbe0  1c0094e5  ldr r0, [r4,#0x1c]
0038fbe4  000050e3  cmp r0, #0x0
0038fbe8  47aaf61b  blne 0x0013a50c
0038fbec  180094e5  ldr r0, [r4,#0x18]
0038fbf0  000050e3  cmp r0, #0x0
0038fbf4  44aaf61b  blne 0x0013a50c
0038fbf8  080094e5  ldr r0, [r4,#0x8]
0038fbfc  000050e3  cmp r0, #0x0
0038fc00  0200000a  beq 0x0038fc10
0038fc04  001090e5  ldr r1, [r0,#0x0]
0038fc08  041091e5  ldr r1, [r1,#0x4]
0038fc0c  31ff2fe1  blx r1
0038fc10  0400a0e1  cpy r0, r4
0038fc14  1040bde8  ldmia sp!,{r4,lr}
0038fc18  40befdea  b 0x002ff520
