108b176c  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b1770  10d04de2  sub sp, sp, #0x10
108b1774  0070a0e1  cpy r7, r0
108b1778  0140a0e1  cpy r4, r1
108b177c  0250a0e1  cpy r5, r2
108b1780  0380a0e1  cpy r8, r3
108b1784  0100a0e3  mov r0, #0x1
108b1788  ab69feeb  bl 0x1084be3c
108b178c  0060a0e1  cpy r6, r0
108b1790  0500a0e1  cpy r0, r5
108b1794  096afeeb  bl 0x1084bfc0
108b1798  a810a0e3  mov r1, #0xa8
108b179c  0c108de5  str r1, [sp,#0xc]
108b17a0  0a10a0e3  mov r1, #0xa
108b17a4  43008de8  stmia sp,{r0,r1,r6}
108b17a8  0830a0e1  cpy r3, r8
108b17ac  0520a0e1  cpy r2, r5
108b17b0  0410a0e1  cpy r1, r4
108b17b4  0700a0e1  cpy r0, r7
108b17b8  3c69feeb  bl 0x1084bcb0
108b17bc  0460a0e1  cpy r6, r4
108b17c0  0510a0e1  cpy r1, r5
108b17c4  0400a0e1  cpy r0, r4
108b17c8  348bfeeb  bl 0x108544a0
108b17cc  0070a0e1  cpy r7, r0
108b17d0  0520a0e1  cpy r2, r5
108b17d4  2710a0e3  mov r1, #0x27
108b17d8  0600a0e1  cpy r0, r6
108b17dc  e6e1feeb  bl 0x10869f7c
108b17e0  0040a0e1  cpy r4, r0
108b17e4  0310a0e3  mov r1, #0x3
108b17e8  0700a0e1  cpy r0, r7
108b17ec  ce68feeb  bl 0x1084bb2c
108b17f0  001094e5  ldr r1, [r4,#0x0]
108b17f4  0320a0e3  mov r2, #0x3
108b17f8  0450c4e5  strb r5, [r4,#0x4]
108b17fc  010080e2  add r0, r0, #0x1
108b1800  0520c4e5  strb r2, [r4,#0x5]
108b1804  0600c4e5  strb r0, [r4,#0x6]
108b1808  010481e3  orr r0, r1, #0x1000000
108b180c  000084e5  str r0, [r4,#0x0]
108b1810  10d08de2  add sp, sp, #0x10
108b1814  0410a0e1  cpy r1, r4
108b1818  0600a0e1  cpy r0, r6
108b181c  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108b1820  c8d8feea  b 0x10867b48
