108ab5a0  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108ab5a4  0150a0e1  cpy r5, r1
108ab5a8  0040a0e1  cpy r4, r0
108ab5ac  0260a0e1  cpy r6, r2
108ab5b0  0210a0e1  cpy r1, r2
108ab5b4  0500a0e1  cpy r0, r5
108ab5b8  b8a3feeb  bl 0x108544a0
108ab5bc  0400a0e1  cpy r0, r4
108ab5c0  0370a0e3  mov r7, #0x3
108ab5c4  b803d0e1  ldrh r0, [r0,#0x38]
108ab5c8  0710a0e1  cpy r1, r7
108ab5cc  6886feeb  bl 0x1084cf74
108ab5d0  ff2000e2  and r2, r0, #0xff
108ab5d4  0610a0e1  cpy r1, r6
108ab5d8  0500a0e1  cpy r0, r5
108ab5dc  5384feeb  bl 0x1084c730
108ab5e0  000050e3  cmp r0, #0x0
108ab5e4  0600000a  beq 0x108ab604
108ab5e8  0400a0e1  cpy r0, r4
108ab5ec  de84feeb  bl 0x1084c96c
108ab5f0  0620a0e1  cpy r2, r6
108ab5f4  0500a0e1  cpy r0, r5
108ab5f8  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108ab5fc  0010a0e3  mov r1, #0x0
108ab600  d084feea  b 0x1084c948
108ab604  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
