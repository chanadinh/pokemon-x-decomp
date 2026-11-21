108aeb70  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108aeb74  0160a0e1  cpy r6, r1
108aeb78  0250a0e1  cpy r5, r2
108aeb7c  0300a0e3  mov r0, #0x3
108aeb80  65f3feeb  bl 0x1086b91c
108aeb84  050050e1  cmp r0, r5
108aeb88  4500001a  bne 0x108aeca4
108aeb8c  0510a0e1  cpy r1, r5
108aeb90  0600a0e1  cpy r0, r6
108aeb94  4196feeb  bl 0x108544a0
108aeb98  0070a0e1  cpy r7, r0
108aeb9c  dd79feeb  bl 0x1084d318
108aeba0  000050e3  cmp r0, #0x0
108aeba4  3e00001a  bne 0x108aeca4
108aeba8  0520a0e1  cpy r2, r5
108aebac  0b10a0e3  mov r1, #0xb
108aebb0  0600a0e1  cpy r0, r6
108aebb4  f0ecfeeb  bl 0x10869f7c
108aebb8  0040a0e1  cpy r4, r0
108aebbc  0200a0e3  mov r0, #0x2
108aebc0  0f50c4e5  strb r5, [r4,#0xf]
108aebc4  b400c4e1  strh r0, [r4,#0x4]
108aebc8  0300a0e3  mov r0, #0x3
108aebcc  9a74feeb  bl 0x1084be3c
108aebd0  080084e5  str r0, [r4,#0x8]
108aebd4  0110a0e3  mov r1, #0x1
108aebd8  0c10c4e5  strb r1, [r4,#0xc]
108aebdc  1010c4e5  strb r1, [r4,#0x10]
108aebe0  0e10c4e5  strb r1, [r4,#0xe]
108aebe4  bc209fe5  ldr r2, [0x108aeca8]
108aebe8  0210a0e3  mov r1, #0x2
108aebec  140084e2  add r0, r4, #0x14
108aebf0  b779feeb  bl 0x1084d2d4
108aebf4  0510a0e1  cpy r1, r5
108aebf8  140084e2  add r0, r4, #0x14
108aebfc  a679feeb  bl 0x1084d29c
108aec00  0410a0e1  cpy r1, r4
108aec04  0600a0e1  cpy r0, r6
108aec08  cee3feeb  bl 0x10867b48
108aec0c  0520a0e1  cpy r2, r5
108aec10  0410a0e3  mov r1, #0x4
108aec14  0600a0e1  cpy r0, r6
108aec18  d7ecfeeb  bl 0x10869f7c
108aec1c  0040a0e1  cpy r4, r0
108aec20  0650c0e5  strb r5, [r0,#0x6]
108aec24  0e10a0e3  mov r1, #0xe
108aec28  0700a0e1  cpy r0, r7
108aec2c  fb15ffeb  bl 0x10874420
108aec30  0080a0e1  cpy r8, r0
108aec34  0d10a0e3  mov r1, #0xd
108aec38  0700a0e1  cpy r0, r7
108aec3c  f715ffeb  bl 0x10874420
108aec40  001094e5  ldr r1, [r4,#0x0]
108aec44  000048e0  sub r0, r8, r0
108aec48  b400c4e1  strh r0, [r4,#0x4]
108aec4c  011481e3  orr r1, r1, #0x1000000
108aec50  001084e5  str r1, [r4,#0x0]
108aec54  0410a0e1  cpy r1, r4
108aec58  0600a0e1  cpy r0, r6
108aec5c  b9e3feeb  bl 0x10867b48
108aec60  0520a0e1  cpy r2, r5
108aec64  2210a0e3  mov r1, #0x22
108aec68  0600a0e1  cpy r0, r6
108aec6c  c2ecfeeb  bl 0x10869f7c
108aec70  002090e5  ldr r2, [r0,#0x0]
108aec74  0010a0e1  cpy r1, r0
108aec78  0400a0e3  mov r0, #0x4
108aec7c  0450c1e5  strb r5, [r1,#0x4]
108aec80  0500c1e5  strb r0, [r1,#0x5]
108aec84  012482e3  orr r2, r2, #0x1000000
108aec88  0600a0e1  cpy r0, r6
108aec8c  002081e5  str r2, [r1,#0x0]
108aec90  ace3feeb  bl 0x10867b48
108aec94  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108aec98  0010a0e3  mov r1, #0x0
108aec9c  5300a0e3  mov r0, #0x53
108aeca0  0bdbfeea  b 0x108658d4
108aeca4  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
