00152ea8  70402de9  stmdb sp!,{r4,r5,r6,lr}
00152eac  0040a0e1  cpy r4,r0
00152eb0  040090e5  ldr r0,[r0,#0x4]
00152eb4  000050e3  cmp r0,#0x0
00152eb8  0b00000a  beq 0x00152eec
00152ebc  0400a0e1  cpy r0,r4
00152ec0  e65500eb  bl 0x00168660
00152ec4  080094e5  ldr r0,[r4,#0x8]
00152ec8  b400d0e1  ldrh r0,[r0,#0x4]
00152ecc  040000e2  and r0,r0,#0x4
00152ed0  2001b0e1  movs r0,r0, lsr #0x2
00152ed4  04009405  ldreq r0,[r4,#0x4]
00152ed8  10009f15  ldrne r0,[0x152ef0]
00152edc  b850d0e1  ldrh r5,[r0,#0x8]
00152ee0  0400a0e1  cpy r0,r4
00152ee4  c15500eb  bl 0x001685f0
00152ee8  0500a0e1  cpy r0,r5
00152eec  7080bde8  ldmia sp!,{r4,r5,r6,pc}
