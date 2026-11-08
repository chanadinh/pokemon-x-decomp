108c4eb4  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108c4eb8  0140a0e1  cpy r4, r1
108c4ebc  0250a0e1  cpy r5, r2
108c4ec0  0210a0e1  cpy r1, r2
108c4ec4  0400a0e1  cpy r0, r4
108c4ec8  1818feeb  bl 0x1084af30
108c4ecc  000050e3  cmp r0, #0x0
108c4ed0  1d00001a  bne 0x108c4f4c
108c4ed4  0200a0e3  mov r0, #0x2
108c4ed8  8f9afeeb  bl 0x1086b91c
108c4edc  ff6000e2  and r6, r0, #0xff
108c4ee0  0510a0e1  cpy r1, r5
108c4ee4  0600a0e1  cpy r0, r6
108c4ee8  ae1dfeeb  bl 0x1084c5a8
108c4eec  000050e3  cmp r0, #0x0
108c4ef0  1500001a  bne 0x108c4f4c
108c4ef4  1200a0e3  mov r0, #0x12
108c4ef8  879afeeb  bl 0x1086b91c
108c4efc  7070ffe6  uxth r7, r0
108c4f00  0700a0e1  cpy r0, r7
108c4f04  32eefceb  bl 0x108007d4
108c4f08  0b0050e3  cmp r0, #0xb
108c4f0c  0e00001a  bne 0x108c4f4c
108c4f10  0410a0e3  mov r1, #0x4
108c4f14  0700a0e1  cpy r0, r7
108c4f18  5fedfceb  bl 0x1080049c
108c4f1c  000050e3  cmp r0, #0x0
108c4f20  0900000a  beq 0x108c4f4c
108c4f24  1910a0e3  mov r1, #0x19
108c4f28  2300a0e3  mov r0, #0x23
108c4f2c  6882feeb  bl 0x108658d4
108c4f30  000050e3  cmp r0, #0x0
108c4f34  0400000a  beq 0x108c4f4c
108c4f38  0620a0e1  cpy r2, r6
108c4f3c  0510a0e1  cpy r1, r5
108c4f40  0400a0e1  cpy r0, r4
108c4f44  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108c4f48  2a19feea  b 0x1084b3f8
108c4f4c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
