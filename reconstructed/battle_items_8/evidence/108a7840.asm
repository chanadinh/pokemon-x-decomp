108a7840  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108a7844  0050a0e1  cpy r5, r0
108a7848  0170a0e1  cpy r7, r1
108a784c  0240a0e1  cpy r4, r2
108a7850  0200a0e3  mov r0, #0x2
108a7854  3010ffeb  bl 0x1086b91c
108a7858  040050e1  cmp r0, r4
108a785c  1700001a  bne 0x108a78c0
108a7860  0410a0e1  cpy r1, r4
108a7864  0700a0e1  cpy r0, r7
108a7868  0cb3feeb  bl 0x108544a0
108a786c  0080a0e1  cpy r8, r0
108a7870  0500a0e1  cpy r0, r5
108a7874  0360a0e3  mov r6, #0x3
108a7878  b803d0e1  ldrh r0, [r0,#0x38]
108a787c  0610a0e1  cpy r1, r6
108a7880  bb95feeb  bl 0x1084cf74
108a7884  ff6000e2  and r6, r0, #0xff
108a7888  0420a0e1  cpy r2, r4
108a788c  0410a0e3  mov r1, #0x4
108a7890  0700a0e1  cpy r0, r7
108a7894  b809ffeb  bl 0x10869f7c
108a7898  0050a0e1  cpy r5, r0
108a789c  0640c0e5  strb r4, [r0,#0x6]
108a78a0  0610a0e1  cpy r1, r6
108a78a4  0800a0e1  cpy r0, r8
108a78a8  9196feeb  bl 0x1084d2f4
108a78ac  b400c5e1  strh r0, [r5,#0x4]
108a78b0  0510a0e1  cpy r1, r5
108a78b4  0700a0e1  cpy r0, r7
108a78b8  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108a78bc  a100ffea  b 0x10867b48
108a78c0  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
