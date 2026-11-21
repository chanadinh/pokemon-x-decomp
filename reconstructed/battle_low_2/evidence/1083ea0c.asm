1083ea0c  4cc09fe5  ldr r12, [0x1083ea60]
1083ea10  70402de9  stmdb sp!,{r4,r5,r6,lr}
1083ea14  0160a0e1  cpy r6, r1
1083ea18  44309fe5  ldr r3, [0x1083ea64]
1083ea1c  6c409ce5  ldr r4, [r12,#0x6c]
1083ea20  0250a0e1  cpy r5, r2
1083ea24  030054e1  cmp r4, r3
1083ea28  6c308c15  strne r3, [r12,#0x6c]
1083ea2c  904190e5  ldr r4, [r0,#0x190]
1083ea30  000094e5  ldr r0, [r4,#0x0]
1083ea34  001090e5  ldr r1, [r0,#0x0]
1083ea38  241091e5  ldr r1, [r1,#0x24]
1083ea3c  31ff2fe1  blx r1
1083ea40  0610a0e1  cpy r1, r6
1083ea44  4ede00eb  bl 0x10876384
1083ea48  9f2fa0e3  mov r2, #0x27c
1083ea4c  780284e5  str r0, [r4,#0x278]
1083ea50  0010a0e3  mov r1, #0x0
1083ea54  b45082e1  strh r5, [r2,r4]
1083ea58  3c1084e5  str r1, [r4,#0x3c]
1083ea5c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
