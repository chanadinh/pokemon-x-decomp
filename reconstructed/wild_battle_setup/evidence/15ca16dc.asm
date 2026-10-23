15ca16dc  f0402de9  stmdb sp!,{r4,r5,r6,r7,lr}
15ca16e0  b4d04de2  sub sp,sp,#0xb4
15ca16e4  0040a0e1  cpy r4,r0
15ca16e8  0150a0e1  cpy r5,r1
15ca16ec  0270a0e1  cpy r7,r2
15ca16f0  0060a0e3  mov r6,#0x0
15ca16f4  08008de2  add r0,sp,#0x8
15ca16f8  c44fffeb  bl 0x15c75610
15ca16fc  480094e5  ldr r0,[r4,#0x48]
15ca1700  000050e3  cmp r0,#0x0
15ca1704  0300000a  beq 0x15ca1718
15ca1708  580190e5  ldr r0,[r0,#0x158]
15ca170c  0c0090e5  ldr r0,[r0,#0xc]
15ca1710  9e7efdeb  bl 0x15c01190
15ca1714  0060a0e1  cpy r6,r0
15ca1718  2c1094e5  ldr r1,[r4,#0x2c]
15ca171c  ff00a0e3  mov r0,#0xff
15ca1720  0630a0e1  cpy r3,r6
15ca1724  0120a0e3  mov r2,#0x1
15ca1728  3410d1e5  ldrb r1,[r1,#0x34]
15ca172c  f000cde1  strd r0,r1,[sp,#0x0]
15ca1730  0410a0e1  cpy r1,r4
15ca1734  08008de2  add r0,sp,#0x8
15ca1738  3d4fffeb  bl 0x15c75434
15ca173c  0100a0e3  mov r0,#0x1
15ca1740  78008de5  str r0,[sp,#0x78]
15ca1744  7c008de5  str r0,[sp,#0x7c]
15ca1748  74008de5  str r0,[sp,#0x74]
15ca174c  84009fe5  ldr r0,[0x15ca17d8]
15ca1750  851085e0  add r1,r5,r5, lsl #0x1
15ca1754  010180e0  add r0,r0,r1, lsl #0x2
15ca1758  0810d0e5  ldrb r1,[r0,#0x8]
15ca175c  9410cde5  strb r1,[sp,#0x94]
15ca1760  0910d0e5  ldrb r1,[r0,#0x9]
15ca1764  9510cde5  strb r1,[sp,#0x95]
15ca1768  0a10d0e5  ldrb r1,[r0,#0xa]
15ca176c  9610cde5  strb r1,[sp,#0x96]
15ca1770  b610d0e1  ldrh r1,[r0,#0x6]
15ca1774  a123a0e1  mov r2,r1, lsr #0x7
15ca1778  01cea0e1  mov r12,r1, lsl #0x1c
15ca177c  9720cde5  strb r2,[sp,#0x97]
15ca1780  2c2fa0e1  mov r2,r12, lsr #0x1e
15ca1784  b0c0d0e1  ldrh r12,[r0,#0x0]
15ca1788  813ca0e1  mov r3,r1, lsl #0x19
15ca178c  031001e2  and r1,r1,#0x3
15ca1790  b8c0cde1  strh r12,[sp,#0x8]
15ca1794  b4c0d0e1  ldrh r12,[r0,#0x4]
15ca1798  a33ea0e1  mov r3,r3, lsr #0x1d
15ca179c  bac0cde1  strh r12,[sp,#0xa]
15ca17a0  03c0d0e5  ldrb r12,[r0,#0x3]
15ca17a4  0cc0cde5  strb r12,[sp,#0xc]
15ca17a8  0200d0e5  ldrb r0,[r0,#0x2]
15ca17ac  1820cde5  strb r2,[sp,#0x18]
15ca17b0  08208de2  add r2,sp,#0x8
15ca17b4  0d00cde5  strb r0,[sp,#0xd]
15ca17b8  0e10cde5  strb r1,[sp,#0xe]
15ca17bc  0f30cde5  strb r3,[sp,#0xf]
15ca17c0  180094e5  ldr r0,[r4,#0x18]
15ca17c4  0730a0e1  cpy r3,r7
15ca17c8  0410a0e1  cpy r1,r4
15ca17cc  2189fdeb  bl 0x15c03c58
15ca17d0  b4d08de2  add sp,sp,#0xb4
15ca17d4  f080bde8  ldmia sp!,{r4,r5,r6,r7,pc}
