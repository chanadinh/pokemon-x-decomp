108a80a8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a80ac  0150a0e1  cpy r5, r1
108a80b0  0240a0e1  cpy r4, r2
108a80b4  0400a0e3  mov r0, #0x4
108a80b8  170effeb  bl 0x1086b91c
108a80bc  040050e1  cmp r0, r4
108a80c0  0e00001a  bne 0x108a8100
108a80c4  0410a0e1  cpy r1, r4
108a80c8  0500a0e1  cpy r0, r5
108a80cc  f3b0feeb  bl 0x108544a0
108a80d0  bc00d0e1  ldrh r0, [r0,#0xc]
108a80d4  011c40e2  sub r1, r0, #0x100
108a80d8  6e1051e2  subs r1, r1, #0x6e
108a80dc  0700001a  bne 0x108a8100
108a80e0  1b00a0e3  mov r0, #0x1b
108a80e4  0c0effeb  bl 0x1086b91c
108a80e8  020050e3  cmp r0, #0x2
108a80ec  0300001a  bne 0x108a8100
108a80f0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a80f4  021aa0e3  mov r1, #0x2000
108a80f8  3600a0e3  mov r0, #0x36
108a80fc  6093feea  b 0x1084ce84
108a8100  7080bde8  ldmia sp!,{r4,r5,r6,pc}
