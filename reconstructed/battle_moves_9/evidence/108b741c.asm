108b741c  f0402de9  stmdb sp!,{r4,r5,r6,r7,lr}
108b7420  0cd04de2  sub sp, sp, #0xc
108b7424  0160a0e1  cpy r6, r1
108b7428  0250a0e1  cpy r5, r2
108b742c  0300a0e3  mov r0, #0x3
108b7430  39d1feeb  bl 0x1086b91c
108b7434  050050e1  cmp r0, r5
108b7438  2600001a  bne 0x108b74d8
108b743c  4800a0e3  mov r0, #0x48
108b7440  35d1feeb  bl 0x1086b91c
108b7444  000050e3  cmp r0, #0x0
108b7448  2200001a  bne 0x108b74d8
108b744c  0600a0e3  mov r0, #0x6
108b7450  31d1feeb  bl 0x1086b91c
108b7454  ff4000e2  and r4, r0, #0xff
108b7458  1e30a0e3  mov r3, #0x1e
108b745c  0420a0e1  cpy r2, r4
108b7460  0510a0e1  cpy r1, r5
108b7464  0600a0e1  cpy r0, r6
108b7468  5ebbfdeb  bl 0x108261e8
108b746c  000050e3  cmp r0, #0x0
108b7470  1800000a  beq 0x108b74d8
108b7474  0510a0e1  cpy r1, r5
108b7478  0600a0e1  cpy r0, r6
108b747c  0774feeb  bl 0x108544a0
108b7480  0070a0e1  cpy r7, r0
108b7484  0d20a0e1  cpy r2, sp
108b7488  04108de2  add r1, sp, #0x4
108b748c  0600a0e1  cpy r0, r6
108b7490  051100eb  bl 0x108bb8ac
108b7494  0400dde5  ldrb r0, [sp,#0x4]
108b7498  000050e3  cmp r0, #0x0
108b749c  0f00000a  beq 0x108b74e0
108b74a0  010050e3  cmp r0, #0x1
108b74a4  1e00000a  beq 0x108b7524
108b74a8  020050e3  cmp r0, #0x2
108b74ac  0900001a  bne 0x108b74d8
108b74b0  0520a0e1  cpy r2, r5
108b74b4  2c10a0e3  mov r1, #0x2c
108b74b8  0600a0e1  cpy r0, r6
108b74bc  aecafeeb  bl 0x10869f7c
108b74c0  0010a0e1  cpy r1, r0
108b74c4  6400a0e3  mov r0, #0x64
108b74c8  0440c1e5  strb r4, [r1,#0x4]
108b74cc  0500c1e5  strb r0, [r1,#0x5]
108b74d0  0600a0e1  cpy r0, r6
108b74d4  9bc1feeb  bl 0x10867b48
108b74d8  0cd08de2  add sp, sp, #0xc
108b74dc  f080bde8  ldmia sp!,{r4,r5,r6,r7,pc}
108b74e0  0520a0e1  cpy r2, r5
108b74e4  0b10a0e3  mov r1, #0xb
108b74e8  0600a0e1  cpy r0, r6
108b74ec  a2cafeeb  bl 0x10869f7c
108b74f0  0f40c0e5  strb r4, [r0,#0xf]
108b74f4  0030dde5  ldrb r3, [sp,#0x0]
108b74f8  0050a0e1  cpy r5, r0
108b74fc  082080e2  add r2, r0, #0x8
108b7500  b430c0e1  strh r3, [r0,#0x4]
108b7504  0710a0e1  cpy r1, r7
108b7508  0300a0e1  cpy r0, r3
108b750c  c953feeb  bl 0x1084c438
108b7510  0510a0e1  cpy r1, r5
108b7514  0600a0e1  cpy r0, r6
108b7518  8ac1feeb  bl 0x10867b48
108b751c  0cd08de2  add sp, sp, #0xc
108b7520  f080bde8  ldmia sp!,{r4,r5,r6,r7,pc}
108b7524  0520a0e1  cpy r2, r5
108b7528  0d10a0e3  mov r1, #0xd
108b752c  0600a0e1  cpy r0, r6
108b7530  91cafeeb  bl 0x10869f7c
108b7534  0010a0e1  cpy r1, r0
108b7538  0100a0e3  mov r0, #0x1
108b753c  0c40c1e5  strb r4, [r1,#0xc]
108b7540  1600c1e5  strb r0, [r1,#0x16]
108b7544  0030dde5  ldrb r3, [sp,#0x0]
108b7548  1800d1e5  ldrb r0, [r1,#0x18]
108b754c  ff20a0e3  mov r2, #0xff
108b7550  0430c1e5  strb r3, [r1,#0x4]
108b7554  040080e3  orr r0, r0, #0x4
108b7558  1720c1e5  strb r2, [r1,#0x17]
108b755c  1800c1e5  strb r0, [r1,#0x18]
108b7560  0600a0e1  cpy r0, r6
108b7564  77c1feeb  bl 0x10867b48
108b7568  0cd08de2  add sp, sp, #0xc
108b756c  f080bde8  ldmia sp!,{r4,r5,r6,r7,pc}
