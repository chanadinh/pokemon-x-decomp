108bc8dc  f8402de9  stmdb sp!,{r3,r4,r5,r6,r7,lr}
108bc8e0  0250a0e1  cpy r5, r2
108bc8e4  0160a0e1  cpy r6, r1
108bc8e8  0400a0e3  mov r0, #0x4
108bc8ec  0abcfeeb  bl 0x1086b91c
108bc8f0  050050e1  cmp r0, r5
108bc8f4  2a00001a  bne 0x108bc9a4
108bc8f8  4700a0e3  mov r0, #0x47
108bc8fc  06bcfeeb  bl 0x1086b91c
108bc900  000050e3  cmp r0, #0x0
108bc904  2600001a  bne 0x108bc9a4
108bc908  0300a0e3  mov r0, #0x3
108bc90c  02bcfeeb  bl 0x1086b91c
108bc910  ff4000e2  and r4, r0, #0xff
108bc914  0410a0e1  cpy r1, r4
108bc918  0600a0e1  cpy r0, r6
108bc91c  df5efeeb  bl 0x108544a0
108bc920  0070a0e1  cpy r7, r0
108bc924  0b10a0e3  mov r1, #0xb
108bc928  0cccfeeb  bl 0x1086f960
108bc92c  000050e3  cmp r0, #0x0
108bc930  1b00001a  bne 0x108bc9a4
108bc934  1110a0e3  mov r1, #0x11
108bc938  0700a0e1  cpy r0, r7
108bc93c  b7defeeb  bl 0x10874420
108bc940  8e0050e3  cmp r0, #0x8e
108bc944  1600000a  beq 0x108bc9a4
108bc948  0410a0e1  cpy r1, r4
108bc94c  0600a0e1  cpy r0, r6
108bc950  3039feeb  bl 0x1084ae18
108bc954  021c40e2  sub r1, r0, #0x200
108bc958  8a1051e2  subs r1, r1, #0x8a
108bc95c  1000000a  beq 0x108bc9a4
108bc960  1e00a0e3  mov r0, #0x1e
108bc964  943ffeeb  bl 0x1084c7bc
108bc968  140050e3  cmp r0, #0x14
108bc96c  0540a083  movhi r4, #0x5
108bc970  0200008a  bhi 0x108bc980
108bc974  0a0050e3  cmp r0, #0xa
108bc978  0140a083  movhi r4, #0x1
108bc97c  0240a093  movls r4, #0x2
108bc980  ff0004e2  and r0, r4, #0xff
108bc984  1d91feeb  bl 0x10860e00
108bc988  1e10a0e3  mov r1, #0x1e
108bc98c  0030a0e1  cpy r3, r0
108bc990  00108de5  str r1, [sp,#0x0]
108bc994  0420a0e1  cpy r2, r4
108bc998  0510a0e1  cpy r1, r5
108bc99c  0600a0e1  cpy r0, r6
108bc9a0  ea38feeb  bl 0x1084ad50
108bc9a4  f880bde8  ldmia sp!,{r3,r4,r5,r6,r7,pc}
