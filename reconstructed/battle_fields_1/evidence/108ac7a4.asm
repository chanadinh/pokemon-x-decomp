108ac7a4  70402de9  stmdb sp!,{r4,r5,r6,lr}
108ac7a8  0240a0e1  cpy r4, r2
108ac7ac  0400a0e3  mov r0, #0x4
108ac7b0  59fcfeeb  bl 0x1086b91c
108ac7b4  ff5000e2  and r5, r0, #0xff
108ac7b8  0500a0e1  cpy r0, r5
108ac7bc  ff7dfeeb  bl 0x1084bfc0
108ac7c0  040050e1  cmp r0, r4
108ac7c4  1f00001a  bne 0x108ac848
108ac7c8  1200a0e3  mov r0, #0x12
108ac7cc  52fcfeeb  bl 0x1086b91c
108ac7d0  7060ffe6  uxth r6, r0
108ac7d4  1c00a0e3  mov r0, #0x1c
108ac7d8  4ffcfeeb  bl 0x1086b91c
108ac7dc  ff4000e2  and r4, r0, #0xff
108ac7e0  0600a0e1  cpy r0, r6
108ac7e4  124ffdeb  bl 0x10800434
108ac7e8  000050e3  cmp r0, #0x0
108ac7ec  1500001a  bne 0x108ac848
108ac7f0  0300a0e3  mov r0, #0x3
108ac7f4  48fcfeeb  bl 0x1086b91c
108ac7f8  050050e1  cmp r0, r5
108ac7fc  1100000a  beq 0x108ac848
108ac800  080054e3  cmp r4, #0x8
108ac804  06005413  cmpne r4, #0x6
108ac808  0e00000a  beq 0x108ac848
108ac80c  0110a0e3  mov r1, #0x1
108ac810  4100a0e3  mov r0, #0x41
108ac814  2ee4feeb  bl 0x108658d4
108ac818  000050e3  cmp r0, #0x0
108ac81c  0900000a  beq 0x108ac848
108ac820  4000a0e3  mov r0, #0x40
108ac824  3cfcfeeb  bl 0x1086b91c
108ac828  1c209fe5  ldr r2, [0x108ac84c]
108ac82c  0040a0e1  cpy r4, r0
108ac830  0210a0e3  mov r1, #0x2
108ac834  a682feeb  bl 0x1084d2d4
108ac838  0510a0e1  cpy r1, r5
108ac83c  0400a0e1  cpy r0, r4
108ac840  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108ac844  9482feea  b 0x1084d29c
108ac848  7080bde8  ldmia sp!,{r4,r5,r6,pc}
