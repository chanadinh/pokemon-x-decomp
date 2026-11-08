108c44ac  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c44b0  0240a0e1  cpy r4, r2
108c44b4  0350a0e1  cpy r5, r3
108c44b8  0300a0e3  mov r0, #0x3
108c44bc  169dfeeb  bl 0x1086b91c
108c44c0  040050e1  cmp r0, r4
108c44c4  0c00001a  bne 0x108c44fc
108c44c8  0c0095e5  ldr r0, [r5,#0xc]
108c44cc  000050e3  cmp r0, #0x0
108c44d0  0900000a  beq 0x108c44fc
108c44d4  1200a0e3  mov r0, #0x12
108c44d8  0f9dfeeb  bl 0x1086b91c
108c44dc  7000ffe6  uxth r0, r0
108c44e0  bdf0fceb  bl 0x108007dc
108c44e4  010050e3  cmp r0, #0x1
108c44e8  0300001a  bne 0x108c44fc
108c44ec  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c44f0  021ba0e3  mov r1, #0x800
108c44f4  3600a0e3  mov r0, #0x36
108c44f8  6122feea  b 0x1084ce84
108c44fc  7080bde8  ldmia sp!,{r4,r5,r6,pc}
