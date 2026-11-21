108acc24  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108acc28  0340a0e1  cpy r4, r3
108acc2c  0170a0e1  cpy r7, r1
108acc30  000093e5  ldr r0, [r3,#0x0]
108acc34  000050e3  cmp r0, #0x0
108acc38  1300000a  beq 0x108acc8c
108acc3c  0400a0e3  mov r0, #0x4
108acc40  35fbfeeb  bl 0x1086b91c
108acc44  ff6000e2  and r6, r0, #0xff
108acc48  0620a0e1  cpy r2, r6
108acc4c  0310a0e3  mov r1, #0x3
108acc50  0700a0e1  cpy r0, r7
108acc54  c8f4feeb  bl 0x10869f7c
108acc58  0050a0e1  cpy r5, r0
108acc5c  2c209fe5  ldr r2, [0x108acc90]
108acc60  0210a0e3  mov r1, #0x2
108acc64  040080e2  add r0, r0, #0x4
108acc68  9981feeb  bl 0x1084d2d4
108acc6c  0610a0e1  cpy r1, r6
108acc70  040085e2  add r0, r5, #0x4
108acc74  8881feeb  bl 0x1084d29c
108acc78  0510a0e1  cpy r1, r5
108acc7c  0700a0e1  cpy r0, r7
108acc80  b0ebfeeb  bl 0x10867b48
108acc84  0000a0e3  mov r0, #0x0
108acc88  000084e5  str r0, [r4,#0x0]
108acc8c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
