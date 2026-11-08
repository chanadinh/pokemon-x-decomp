108b9e78  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b9e7c  0150a0e1  cpy r5, r1
108b9e80  0100a0e1  cpy r0, r1
108b9e84  0240a0e1  cpy r4, r2
108b9e88  0210a0e1  cpy r1, r2
108b9e8c  9346feeb  bl 0x1084b8e0
108b9e90  000050e3  cmp r0, #0x0
108b9e94  1100000a  beq 0x108b9ee0
108b9e98  1b10a0e3  mov r1, #0x1b
108b9e9c  2300a0e3  mov r0, #0x23
108b9ea0  8baefeeb  bl 0x108658d4
108b9ea4  000050e3  cmp r0, #0x0
108b9ea8  0c00000a  beq 0x108b9ee0
108b9eac  0420a0e1  cpy r2, r4
108b9eb0  2710a0e3  mov r1, #0x27
108b9eb4  0500a0e1  cpy r0, r5
108b9eb8  2fc0feeb  bl 0x10869f7c
108b9ebc  0010a0e1  cpy r1, r0
108b9ec0  0320a0e3  mov r2, #0x3
108b9ec4  0520c1e5  strb r2, [r1,#0x5]
108b9ec8  0000a0e3  mov r0, #0x0
108b9ecc  0440c1e5  strb r4, [r1,#0x4]
108b9ed0  0600c1e5  strb r0, [r1,#0x6]
108b9ed4  0500a0e1  cpy r0, r5
108b9ed8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b9edc  19b7feea  b 0x10867b48
108b9ee0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
