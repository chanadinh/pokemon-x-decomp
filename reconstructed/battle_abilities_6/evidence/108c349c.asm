108c349c  10402de9  stmdb sp!,{r4,lr}
108c34a0  0240a0e1  cpy r4, r2
108c34a4  0200a0e3  mov r0, #0x2
108c34a8  1ba1feeb  bl 0x1086b91c
108c34ac  040050e1  cmp r0, r4
108c34b0  0e00001a  bne 0x108c34f0
108c34b4  1e00a0e3  mov r0, #0x1e
108c34b8  17a1feeb  bl 0x1086b91c
108c34bc  7000ffe6  uxth r0, r0
108c34c0  040050e3  cmp r0, #0x4
108c34c4  0900001a  bne 0x108c34f0
108c34c8  3300a0e3  mov r0, #0x33
108c34cc  12a1feeb  bl 0x1086b91c
108c34d0  a00f80e0  add r0, r0, r0, lsr #0x1f
108c34d4  0110a0e3  mov r1, #0x1
108c34d8  c000a0e1  mov r0, r0, asr #0x1
108c34dc  866cffeb  bl 0x1089e6fc
108c34e0  0010a0e1  cpy r1, r0
108c34e4  1040bde8  ldmia sp!,{r4,lr}
108c34e8  3300a0e3  mov r0, #0x33
108c34ec  f888feea  b 0x108658d4
108c34f0  1080bde8  ldmia sp!,{r4,pc}
