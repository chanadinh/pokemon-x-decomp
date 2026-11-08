108bc800  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bc804  0150a0e1  cpy r5, r1
108bc808  0240a0e1  cpy r4, r2
108bc80c  0200a0e3  mov r0, #0x2
108bc810  41bcfeeb  bl 0x1086b91c
108bc814  040050e1  cmp r0, r4
108bc818  1e00001a  bne 0x108bc898
108bc81c  0410a0e1  cpy r1, r4
108bc820  0500a0e1  cpy r0, r5
108bc824  1d5ffeeb  bl 0x108544a0
108bc828  1fc4feeb  bl 0x1086d8ac
108bc82c  000050e3  cmp r0, #0x0
108bc830  1800000a  beq 0x108bc898
108bc834  2100a0e3  mov r0, #0x21
108bc838  2e3efeeb  bl 0x1084c0f8
108bc83c  000050e3  cmp r0, #0x0
108bc840  0400001a  bne 0x108bc858
108bc844  0110a0e3  mov r1, #0x1
108bc848  0500a0e1  cpy r0, r5
108bc84c  263efeeb  bl 0x1084c0ec
108bc850  000050e3  cmp r0, #0x0
108bc854  0f00000a  beq 0x108bc898
108bc858  0420a0e1  cpy r2, r4
108bc85c  0a10a0e3  mov r1, #0xa
108bc860  0500a0e1  cpy r0, r5
108bc864  c4b5feeb  bl 0x10869f7c
108bc868  0010a0e1  cpy r1, r0
108bc86c  000090e5  ldr r0, [r0,#0x0]
108bc870  2630a0e3  mov r3, #0x26
108bc874  0540c1e5  strb r4, [r1,#0x5]
108bc878  0120a0e3  mov r2, #0x1
108bc87c  0430c1e5  strb r3, [r1,#0x4]
108bc880  0a0580e3  orr r0, r0, #0x2800000
108bc884  1120c1e5  strb r2, [r1,#0x11]
108bc888  000081e5  str r0, [r1,#0x0]
108bc88c  0500a0e1  cpy r0, r5
108bc890  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bc894  abacfeea  b 0x10867b48
108bc898  7080bde8  ldmia sp!,{r4,r5,r6,pc}
