0012b6a4  10402de9  stmdb sp!,{r4,lr}
0012b6a8  0040a0e1  cpy r4, r0
0012b6ac  18009fe5  ldr r0, [0x12b6cc]
0012b6b0  0010e0e3  mvn r1, #0x0
0012b6b4  f000c4e1  strd r0, r1, [r4,#0x0]
0012b6b8  651fa0e3  mov r1, #0x194
0012b6bc  080084e2  add r0, r4, #0x8
0012b6c0  f55807eb  bl 0x00301a9c
0012b6c4  0400a0e1  cpy r0, r4
0012b6c8  1080bde8  ldmia sp!,{r4,pc}
