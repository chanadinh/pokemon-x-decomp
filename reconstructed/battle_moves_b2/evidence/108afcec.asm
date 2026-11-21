108afcec  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108afcf0  0160a0e1  cpy r6, r1
108afcf4  0240a0e1  cpy r4, r2
108afcf8  0300a0e3  mov r0, #0x3
108afcfc  06effeeb  bl 0x1086b91c
108afd00  040050e1  cmp r0, r4
108afd04  1900001a  bne 0x108afd70
108afd08  0410a0e1  cpy r1, r4
108afd0c  0600a0e1  cpy r0, r6
108afd10  e291feeb  bl 0x108544a0
108afd14  0070a0e1  cpy r7, r0
108afd18  0010a0e3  mov r1, #0x0
108afd1c  d197feeb  bl 0x10855c68
108afd20  c541fdeb  bl 0x1080043c
108afd24  0050a0e1  cpy r5, r0
108afd28  0010a0e1  cpy r1, r0
108afd2c  0700a0e1  cpy r0, r7
108afd30  0afffeeb  bl 0x1086f960
108afd34  000050e3  cmp r0, #0x0
108afd38  0c00001a  bne 0x108afd70
108afd3c  0420a0e1  cpy r2, r4
108afd40  1310a0e3  mov r1, #0x13
108afd44  0600a0e1  cpy r0, r6
108afd48  8be8feeb  bl 0x10869f7c
108afd4c  0010a0e1  cpy r1, r0
108afd50  1f0005e2  and r0, r5, #0x1f
108afd54  0640c1e5  strb r4, [r1,#0x6]
108afd58  800280e1  orr r0, r0, r0, lsl #0x5
108afd5c  120b80e3  orr r0, r0, #0x4800
108afd60  b400c1e1  strh r0, [r1,#0x4]
108afd64  0600a0e1  cpy r0, r6
108afd68  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108afd6c  75dffeea  b 0x10867b48
108afd70  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
