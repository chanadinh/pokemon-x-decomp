1083cb44  5c109fe5  ldr r1, [0x1083cba8]
1083cb48  10402de9  stmdb sp!,{r4,lr}
1083cb4c  0040a0e1  cpy r4, r0
1083cb50  54009fe5  ldr r0, [0x1083cbac]
1083cb54  6c2091e5  ldr r2, [r1,#0x6c]
1083cb58  000052e1  cmp r2, r0
1083cb5c  6c008115  strne r0, [r1,#0x6c]
1083cb60  b80694e5  ldr r0, [r4,#0x6b8]
1083cb64  ff0050e3  cmp r0, #0xff
1083cb68  0100a003  moveq r0, #0x1
1083cb6c  0c00000a  beq 0x1083cba4
1083cb70  0010a0e3  mov r1, #0x0
1083cb74  0400a0e1  cpy r0, r4
1083cb78  4e7000eb  bl 0x10858cb8
1083cb7c  000050e3  cmp r0, #0x0
1083cb80  0700000a  beq 0x1083cba4
1083cb84  0020a0e3  mov r2, #0x0
1083cb88  0210a0e1  cpy r1, r2
1083cb8c  0400a0e1  cpy r0, r4
1083cb90  dd8300eb  bl 0x1085db0c
1083cb94  900194e5  ldr r0, [r4,#0x190]
1083cb98  0a0d80e2  add r0, r0, #0x280
1083cb9c  487400eb  bl 0x10859cc4
1083cba0  0100a0e3  mov r0, #0x1
1083cba4  1080bde8  ldmia sp!,{r4,pc}
