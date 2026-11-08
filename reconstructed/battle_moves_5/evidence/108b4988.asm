108b4988  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b498c  0150a0e1  cpy r5, r1
108b4990  0240a0e1  cpy r4, r2
108b4994  0300a0e3  mov r0, #0x3
108b4998  dfdbfeeb  bl 0x1086b91c
108b499c  040050e1  cmp r0, r4
108b49a0  1e00001a  bne 0x108b4a20
108b49a4  0610a0e3  mov r1, #0x6
108b49a8  0500a0e1  cpy r0, r5
108b49ac  705efeeb  bl 0x1084c374
108b49b0  000050e3  cmp r0, #0x0
108b49b4  0660a0e3  mov r6, #0x6
108b49b8  0420a0e1  cpy r2, r4
108b49bc  0700000a  beq 0x108b49e0
108b49c0  1d10a0e3  mov r1, #0x1d
108b49c4  0500a0e1  cpy r0, r5
108b49c8  6bd5feeb  bl 0x10869f7c
108b49cc  0010a0e1  cpy r1, r0
108b49d0  0460c0e5  strb r6, [r0,#0x4]
108b49d4  0500a0e1  cpy r0, r5
108b49d8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b49dc  59ccfeea  b 0x10867b48
108b49e0  1c10a0e3  mov r1, #0x1c
108b49e4  0500a0e1  cpy r0, r5
108b49e8  63d5feeb  bl 0x10869f7c
108b49ec  0040a0e1  cpy r4, r0
108b49f0  0460c0e5  strb r6, [r0,#0x4]
108b49f4  0500a0e3  mov r0, #0x5
108b49f8  0f5dfeeb  bl 0x1084be3c
108b49fc  080084e5  str r0, [r4,#0x8]
108b4a00  b820a0e3  mov r2, #0xb8
108b4a04  0110a0e3  mov r1, #0x1
108b4a08  0c0084e2  add r0, r4, #0xc
108b4a0c  3062feeb  bl 0x1084d2d4
108b4a10  0410a0e1  cpy r1, r4
108b4a14  0500a0e1  cpy r0, r5
108b4a18  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b4a1c  49ccfeea  b 0x10867b48
108b4a20  7080bde8  ldmia sp!,{r4,r5,r6,pc}
