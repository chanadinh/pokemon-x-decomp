1084db70  70402de9  stmdb sp!,{r4,r5,r6,lr}
1084db74  0040a0e1  cpy r4, r0
1084db78  0150a0e1  cpy r5, r1
1084db7c  000090e5  ldr r0, [r0,#0x0]
1084db80  68109fe5  ldr r1, [0x1084dbf0]
1084db84  010050e1  cmp r0, r1
1084db88  0300003a  bcc 0x1084db9c
1084db8c  0020a0e3  mov r2, #0x0
1084db90  0210a0e1  cpy r1, r2
1084db94  0200a0e1  cpy r0, r2
1084db98  01cafeeb  bl 0x108003a4
1084db9c  001094e5  ldr r1, [r4,#0x0]
1084dba0  080084e2  add r0, r4, #0x8
1084dba4  25cca0e1  mov r12, r5, lsr #0x18
1084dba8  016081e2  add r6, r1, #0x1
1084dbac  006084e5  str r6, [r4,#0x0]
1084dbb0  00c0c1e7  strb r12, [r1,r0]
1084dbb4  001094e5  ldr r1, [r4,#0x0]
1084dbb8  2528a0e1  mov r2, r5, lsr #0x10
1084dbbc  2534a0e1  mov r3, r5, lsr #0x8
1084dbc0  01c081e2  add r12, r1, #0x1
1084dbc4  00c084e5  str r12, [r4,#0x0]
1084dbc8  0020c1e7  strb r2, [r1,r0]
1084dbcc  001094e5  ldr r1, [r4,#0x0]
1084dbd0  012081e2  add r2, r1, #0x1
1084dbd4  002084e5  str r2, [r4,#0x0]
1084dbd8  0030c1e7  strb r3, [r1,r0]
1084dbdc  001094e5  ldr r1, [r4,#0x0]
1084dbe0  012081e2  add r2, r1, #0x1
1084dbe4  002084e5  str r2, [r4,#0x0]
1084dbe8  0050c1e7  strb r5, [r1,r0]
1084dbec  7080bde8  ldmia sp!,{r4,r5,r6,pc}
