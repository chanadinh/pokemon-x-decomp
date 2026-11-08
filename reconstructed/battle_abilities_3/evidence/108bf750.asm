108bf750  f04f2de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11,lr}
108bf754  0cd04de2  sub sp, sp, #0xc
108bf758  0280a0e1  cpy r8, r2
108bf75c  0190a0e1  cpy r9, r1
108bf760  0200a0e3  mov r0, #0x2
108bf764  6cb0feeb  bl 0x1086b91c
108bf768  080050e1  cmp r0, r8
108bf76c  3a00001a  bne 0x108bf85c
108bf770  0070a0e3  mov r7, #0x0
108bf774  0810a0e1  cpy r1, r8
108bf778  0900a0e1  cpy r0, r9
108bf77c  fd34feeb  bl 0x1084cb78
108bf780  011c80e3  orr r1, r0, #0x100
108bf784  0d20a0e1  cpy r2, sp
108bf788  0900a0e1  cpy r0, r9
108bf78c  2331feeb  bl 0x1084bc20
108bf790  006050e2  subs r6, r0, #0x0
108bf794  0040a0e3  mov r4, #0x0
108bf798  0db0a081  cpyhi r11, sp
108bf79c  2e00009a  bls 0x108bf85c
108bf7a0  0410dbe7  ldrb r1, [r11,r4]
108bf7a4  0900a0e1  cpy r0, r9
108bf7a8  3c53feeb  bl 0x108544a0
108bf7ac  00a0a0e1  cpy r10, r0
108bf7b0  0210a0e3  mov r1, #0x2
108bf7b4  b35cfeeb  bl 0x10856a88
108bf7b8  000050e3  cmp r0, #0x0
108bf7bc  00f020e3  nop
108bf7c0  1b00000a  beq 0x108bf834
108bf7c4  000057e3  cmp r7, #0x0
108bf7c8  0400001a  bne 0x108bf7e0
108bf7cc  0170a0e3  mov r7, #0x1
108bf7d0  0820a0e1  cpy r2, r8
108bf7d4  0710a0e1  cpy r1, r7
108bf7d8  0900a0e1  cpy r0, r9
108bf7dc  5934feeb  bl 0x1084c948
108bf7e0  0820a0e1  cpy r2, r8
108bf7e4  0610a0e3  mov r1, #0x6
108bf7e8  0900a0e1  cpy r0, r9
108bf7ec  e2a9feeb  bl 0x10869f7c
108bf7f0  0050a0e1  cpy r5, r0
108bf7f4  0400dbe7  ldrb r0, [r11,r4]
108bf7f8  0810a0e3  mov r1, #0x8
108bf7fc  0600c5e5  strb r0, [r5,#0x6]
108bf800  0a00a0e1  cpy r0, r10
108bf804  ba36feeb  bl 0x1084d2f4
108bf808  b400c5e1  strh r0, [r5,#0x4]
108bf80c  792fa0e3  mov r2, #0x1e4
108bf810  0210a0e3  mov r1, #0x2
108bf814  0c0085e2  add r0, r5, #0xc
108bf818  ad36feeb  bl 0x1084d2d4
108bf81c  0410dbe7  ldrb r1, [r11,r4]
108bf820  0c0085e2  add r0, r5, #0xc
108bf824  9c36feeb  bl 0x1084d29c
108bf828  0510a0e1  cpy r1, r5
108bf82c  0900a0e1  cpy r0, r9
108bf830  c4a0feeb  bl 0x10867b48
108bf834  010084e2  add r0, r4, #0x1
108bf838  ff4000e2  and r4, r0, #0xff
108bf83c  040056e1  cmp r6, r4
108bf840  d6ffff8a  bhi 0x108bf7a0
108bf844  000057e3  cmp r7, #0x0
108bf848  0300000a  beq 0x108bf85c
108bf84c  0820a0e1  cpy r2, r8
108bf850  0210a0e3  mov r1, #0x2
108bf854  0900a0e1  cpy r0, r9
108bf858  3a34feeb  bl 0x1084c948
108bf85c  0cd08de2  add sp, sp, #0xc
108bf860  f08fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,pc}
