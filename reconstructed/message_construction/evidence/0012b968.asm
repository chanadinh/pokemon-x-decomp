0012b968  70402de9  stmdb sp!,{r4,r5,r6,lr}
0012b96c  0040a0e1  cpy r4, r0
0012b970  4c009fe5  ldr r0, [0x12b9c4]
0012b974  0050a0e3  mov r5, #0x0
0012b978  000084e5  str r0, [r4,#0x0]
0012b97c  b450c4e1  strh r5, [r4,#0x4]
0012b980  2400a0e3  mov r0, #0x24
0012b984  085084e5  str r5, [r4,#0x8]
0012b988  e4dbffeb  bl 0x00122920
0012b98c  000050e3  cmp r0, #0x0
0012b990  0800000a  beq 0x0012b9b8
0012b994  005080e5  str r5, [r0,#0x0]
0012b998  045080e5  str r5, [r0,#0x4]
0012b99c  085080e5  str r5, [r0,#0x8]
0012b9a0  0c5080e5  str r5, [r0,#0xc]
0012b9a4  105080e5  str r5, [r0,#0x10]
0012b9a8  145080e5  str r5, [r0,#0x14]
0012b9ac  185080e5  str r5, [r0,#0x18]
0012b9b0  1c5080e5  str r5, [r0,#0x1c]
0012b9b4  205080e5  str r5, [r0,#0x20]
0012b9b8  080084e5  str r0, [r4,#0x8]
0012b9bc  0400a0e1  cpy r0, r4
0012b9c0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
