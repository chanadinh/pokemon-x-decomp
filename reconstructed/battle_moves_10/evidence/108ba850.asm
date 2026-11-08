108ba850  70402de9  stmdb sp!,{r4,r5,r6,lr}
108ba854  0150a0e1  cpy r5, r1
108ba858  0240a0e1  cpy r4, r2
108ba85c  0200a0e3  mov r0, #0x2
108ba860  2dc4feeb  bl 0x1086b91c
108ba864  040050e1  cmp r0, r4
108ba868  1000001a  bne 0x108ba8b0
108ba86c  0410a0e1  cpy r1, r4
108ba870  0500a0e1  cpy r0, r5
108ba874  0967feeb  bl 0x108544a0
108ba878  0040a0e1  cpy r4, r0
108ba87c  1510a0e3  mov r1, #0x15
108ba880  8070feeb  bl 0x10856a88
108ba884  000050e3  cmp r0, #0x0
108ba888  0400001a  bne 0x108ba8a0
108ba88c  1f10a0e3  mov r1, #0x1f
108ba890  0400a0e1  cpy r0, r4
108ba894  7b70feeb  bl 0x10856a88
108ba898  000050e3  cmp r0, #0x0
108ba89c  0300000a  beq 0x108ba8b0
108ba8a0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108ba8a4  1b10a0e3  mov r1, #0x1b
108ba8a8  2300a0e3  mov r0, #0x23
108ba8ac  08acfeea  b 0x108658d4
108ba8b0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
