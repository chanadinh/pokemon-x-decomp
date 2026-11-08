108ba948  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108ba94c  0040a0e1  cpy r4, r0
108ba950  0170a0e1  cpy r7, r1
108ba954  0250a0e1  cpy r5, r2
108ba958  0300a0e3  mov r0, #0x3
108ba95c  eec3feeb  bl 0x1086b91c
108ba960  050050e1  cmp r0, r5
108ba964  1500001a  bne 0x108ba9c0
108ba968  0400a0e1  cpy r0, r4
108ba96c  b803d0e1  ldrh r0, [r0,#0x38]
108ba970  021c40e2  sub r1, r0, #0x200
108ba974  291051e2  subs r1, r1, #0x29
108ba978  44609f05  ldreq r6, [0x108ba9c4]
108ba97c  44609f15  ldrne r6, [0x108ba9c8]
108ba980  0520a0e1  cpy r2, r5
108ba984  0310a0e3  mov r1, #0x3
108ba988  0700a0e1  cpy r0, r7
108ba98c  7abdfeeb  bl 0x10869f7c
108ba990  0040a0e1  cpy r4, r0
108ba994  0620a0e1  cpy r2, r6
108ba998  0210a0e3  mov r1, #0x2
108ba99c  040080e2  add r0, r0, #0x4
108ba9a0  4b4afeeb  bl 0x1084d2d4
108ba9a4  0510a0e1  cpy r1, r5
108ba9a8  040084e2  add r0, r4, #0x4
108ba9ac  3a4afeeb  bl 0x1084d29c
108ba9b0  0410a0e1  cpy r1, r4
108ba9b4  0700a0e1  cpy r0, r7
108ba9b8  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108ba9bc  61b4feea  b 0x10867b48
108ba9c0  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
