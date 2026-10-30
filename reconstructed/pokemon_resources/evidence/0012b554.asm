0012b554  70402de9  stmdb sp!,{r4,r5,r6,lr}
0012b558  0040a0e1  cpy r4, r0
0012b55c  6c009fe5  ldr r0, [0x12b5d0]
0012b560  0050a0e3  mov r5, #0x0
0012b564  000084e5  str r0, [r4,#0x0]
0012b568  b450c4e1  strh r5, [r4,#0x4]
0012b56c  0650c4e5  strb r5, [r4,#0x6]
0012b570  4000a0e3  mov r0, #0x40
0012b574  085084e5  str r5, [r4,#0x8]
0012b578  e8dcffeb  bl 0x00122920
0012b57c  000050e3  cmp r0, #0x0
0012b580  0f00000a  beq 0x0012b5c4
0012b584  005080e5  str r5, [r0,#0x0]
0012b588  045080e5  str r5, [r0,#0x4]
0012b58c  085080e5  str r5, [r0,#0x8]
0012b590  0c5080e5  str r5, [r0,#0xc]
0012b594  105080e5  str r5, [r0,#0x10]
0012b598  145080e5  str r5, [r0,#0x14]
0012b59c  185080e5  str r5, [r0,#0x18]
0012b5a0  1c5080e5  str r5, [r0,#0x1c]
0012b5a4  205080e5  str r5, [r0,#0x20]
0012b5a8  245080e5  str r5, [r0,#0x24]
0012b5ac  285080e5  str r5, [r0,#0x28]
0012b5b0  2c5080e5  str r5, [r0,#0x2c]
0012b5b4  305080e5  str r5, [r0,#0x30]
0012b5b8  345080e5  str r5, [r0,#0x34]
0012b5bc  385080e5  str r5, [r0,#0x38]
0012b5c0  3c5080e5  str r5, [r0,#0x3c]
0012b5c4  080084e5  str r0, [r4,#0x8]
0012b5c8  0400a0e1  cpy r0, r4
0012b5cc  7080bde8  ldmia sp!,{r4,r5,r6,pc}
