108bb270  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108bb274  0160a0e1  cpy r6, r1
108bb278  0070a0e1  cpy r7, r0
108bb27c  0250a0e1  cpy r5, r2
108bb280  0100a0e1  cpy r0, r1
108bb284  2243feeb  bl 0x1084bf14
108bb288  000050e3  cmp r0, #0x0
108bb28c  4300000a  beq 0x108bb3a0
108bb290  0600a0e1  cpy r0, r6
108bb294  1e43feeb  bl 0x1084bf14
108bb298  030050e3  cmp r0, #0x3
108bb29c  3f00000a  beq 0x108bb3a0
108bb2a0  0300a0e3  mov r0, #0x3
108bb2a4  9cc1feeb  bl 0x1086b91c
108bb2a8  ff4000e2  and r4, r0, #0xff
108bb2ac  0510a0e1  cpy r1, r5
108bb2b0  0600a0e1  cpy r0, r6
108bb2b4  7964feeb  bl 0x108544a0
108bb2b8  0080a0e1  cpy r8, r0
108bb2bc  0410a0e1  cpy r1, r4
108bb2c0  0600a0e1  cpy r0, r6
108bb2c4  7564feeb  bl 0x108544a0
108bb2c8  0090a0e1  cpy r9, r0
108bb2cc  0410a0e1  cpy r1, r4
108bb2d0  0500a0e1  cpy r0, r5
108bb2d4  b344feeb  bl 0x1084c5a8
108bb2d8  000050e3  cmp r0, #0x0
108bb2dc  2f00001a  bne 0x108bb3a0
108bb2e0  1200a0e3  mov r0, #0x12
108bb2e4  8cc1feeb  bl 0x1086b91c
108bb2e8  011c40e2  sub r1, r0, #0x100
108bb2ec  fb1051e2  subs r1, r1, #0xfb
108bb2f0  2a00000a  beq 0x108bb3a0
108bb2f4  2110a0e3  mov r1, #0x21
108bb2f8  0800a0e1  cpy r0, r8
108bb2fc  e16dfeeb  bl 0x10856a88
108bb300  000050e3  cmp r0, #0x0
108bb304  2500001a  bne 0x108bb3a0
108bb308  0600a0e1  cpy r0, r6
108bb30c  4343feeb  bl 0x1084c020
108bb310  000050e3  cmp r0, #0x0
108bb314  2100001a  bne 0x108bb3a0
108bb318  0700a0e1  cpy r0, r7
108bb31c  b803d0e1  ldrh r0, [r0,#0x38]
108bb320  770f50e3  cmp r0, #0x1dc
108bb324  0f00001a  bne 0x108bb368
108bb328  0b10a0e3  mov r1, #0xb
108bb32c  0900a0e1  cpy r0, r9
108bb330  8ad1feeb  bl 0x1086f960
108bb334  000050e3  cmp r0, #0x0
108bb338  1800001a  bne 0x108bb3a0
108bb33c  1110a0e3  mov r1, #0x11
108bb340  0900a0e1  cpy r0, r9
108bb344  35e4feeb  bl 0x10874420
108bb348  8e0050e3  cmp r0, #0x8e
108bb34c  1300000a  beq 0x108bb3a0
108bb350  0410a0e1  cpy r1, r4
108bb354  0600a0e1  cpy r0, r6
108bb358  ae3efeeb  bl 0x1084ae18
108bb35c  021c40e2  sub r1, r0, #0x200
108bb360  8a1051e2  subs r1, r1, #0x8a
108bb364  0d00000a  beq 0x108bb3a0
108bb368  1200a0e3  mov r0, #0x12
108bb36c  6ac1feeb  bl 0x1086b91c
108bb370  7030ffe6  uxth r3, r0
108bb374  0520a0e1  cpy r2, r5
108bb378  0410a0e1  cpy r1, r4
108bb37c  0600a0e1  cpy r0, r6
108bb380  bd40feeb  bl 0x1084b67c
108bb384  000050e3  cmp r0, #0x0
108bb388  00f020e3  nop
108bb38c  0300001a  bne 0x108bb3a0
108bb390  0510a0e1  cpy r1, r5
108bb394  f047bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108bb398  0400a0e3  mov r0, #0x4
108bb39c  4ca9feea  b 0x108658d4
108bb3a0  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
