108c11ec  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c11f0  0150a0e1  cpy r5, r1
108c11f4  0240a0e1  cpy r4, r2
108c11f8  0200a0e3  mov r0, #0x2
108c11fc  c6a9feeb  bl 0x1086b91c
108c1200  040050e1  cmp r0, r4
108c1204  0d00001a  bne 0x108c1240
108c1208  0420a0e1  cpy r2, r4
108c120c  0a10a0e3  mov r1, #0xa
108c1210  0500a0e1  cpy r0, r5
108c1214  58a3feeb  bl 0x10869f7c
108c1218  0010a0e1  cpy r1, r0
108c121c  2600a0e3  mov r0, #0x26
108c1220  0400c1e5  strb r0, [r1,#0x4]
108c1224  0120a0e3  mov r2, #0x1
108c1228  1120c1e5  strb r2, [r1,#0x11]
108c122c  1220c1e5  strb r2, [r1,#0x12]
108c1230  0500a0e1  cpy r0, r5
108c1234  0540c1e5  strb r4, [r1,#0x5]
108c1238  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c123c  419afeea  b 0x10867b48
108c1240  7080bde8  ldmia sp!,{r4,r5,r6,pc}
