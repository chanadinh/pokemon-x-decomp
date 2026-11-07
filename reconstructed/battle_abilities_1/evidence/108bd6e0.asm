108bd6e0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bd6e4  0150a0e1  cpy r5, r1
108bd6e8  0240a0e1  cpy r4, r2
108bd6ec  0300a0e3  mov r0, #0x3
108bd6f0  89b8feeb  bl 0x1086b91c
108bd6f4  040050e1  cmp r0, r4
108bd6f8  3200001a  bne 0x108bd7c8
108bd6fc  4700a0e3  mov r0, #0x47
108bd700  85b8feeb  bl 0x1086b91c
108bd704  000050e3  cmp r0, #0x0
108bd708  2e00001a  bne 0x108bd7c8
108bd70c  4800a0e3  mov r0, #0x48
108bd710  81b8feeb  bl 0x1086b91c
108bd714  000050e3  cmp r0, #0x0
108bd718  2a00001a  bne 0x108bd7c8
108bd71c  1200a0e3  mov r0, #0x12
108bd720  7db8feeb  bl 0x1086b91c
108bd724  7000ffe6  uxth r0, r0
108bd728  0010a0e3  mov r1, #0x0
108bd72c  5a0bfdeb  bl 0x1080049c
108bd730  000050e3  cmp r0, #0x0
108bd734  2300000a  beq 0x108bd7c8
108bd738  1e00a0e3  mov r0, #0x1e
108bd73c  6d3afeeb  bl 0x1084c0f8
108bd740  000050e3  cmp r0, #0x0
108bd744  0400001a  bne 0x108bd75c
108bd748  0110a0e3  mov r1, #0x1
108bd74c  0500a0e1  cpy r0, r5
108bd750  653afeeb  bl 0x1084c0ec
108bd754  000050e3  cmp r0, #0x0
108bd758  1a00000a  beq 0x108bd7c8
108bd75c  0420a0e1  cpy r2, r4
108bd760  0b10a0e3  mov r1, #0xb
108bd764  0500a0e1  cpy r0, r5
108bd768  03b2feeb  bl 0x10869f7c
108bd76c  0040a0e1  cpy r4, r0
108bd770  000090e5  ldr r0, [r0,#0x0]
108bd774  020580e3  orr r0, r0, #0x800000
108bd778  000084e5  str r0, [r4,#0x0]
108bd77c  0400a0e3  mov r0, #0x4
108bd780  65b8feeb  bl 0x1086b91c
108bd784  0010a0e1  cpy r1, r0
108bd788  0500a0e3  mov r0, #0x5
108bd78c  0f10c4e5  strb r1, [r4,#0xf]
108bd790  b400c4e1  strh r0, [r4,#0x4]
108bd794  998dfeeb  bl 0x10860e00
108bd798  080084e5  str r0, [r4,#0x8]
108bd79c  762fa0e3  mov r2, #0x1d8
108bd7a0  0210a0e3  mov r1, #0x2
108bd7a4  140084e2  add r0, r4, #0x14
108bd7a8  c93efeeb  bl 0x1084d2d4
108bd7ac  0f10d4e5  ldrb r1, [r4,#0xf]
108bd7b0  140084e2  add r0, r4, #0x14
108bd7b4  b83efeeb  bl 0x1084d29c
108bd7b8  0410a0e1  cpy r1, r4
108bd7bc  0500a0e1  cpy r0, r5
108bd7c0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bd7c4  dfa8feea  b 0x10867b48
108bd7c8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
