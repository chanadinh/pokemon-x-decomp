108ab100  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108ab104  0150a0e1  cpy r5, r1
108ab108  0040a0e1  cpy r4, r0
108ab10c  0260a0e1  cpy r6, r2
108ab110  0210a0e1  cpy r1, r2
108ab114  0500a0e1  cpy r0, r5
108ab118  e0a4feeb  bl 0x108544a0
108ab11c  0400a0e1  cpy r0, r4
108ab120  0370a0e3  mov r7, #0x3
108ab124  b803d0e1  ldrh r0, [r0,#0x38]
108ab128  0710a0e1  cpy r1, r7
108ab12c  9087feeb  bl 0x1084cf74
108ab130  0020a0e1  cpy r2, r0
108ab134  0610a0e1  cpy r1, r6
108ab138  0500a0e1  cpy r0, r5
108ab13c  7b85feeb  bl 0x1084c730
108ab140  000050e3  cmp r0, #0x0
108ab144  0600000a  beq 0x108ab164
108ab148  0400a0e1  cpy r0, r4
108ab14c  0686feeb  bl 0x1084c96c
108ab150  0620a0e1  cpy r2, r6
108ab154  0500a0e1  cpy r0, r5
108ab158  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108ab15c  0010a0e3  mov r1, #0x0
108ab160  f885feea  b 0x1084c948
108ab164  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
