108bb43c  f05f2de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108bb440  0300a0e3  mov r0, #0x3
108bb444  0170a0e1  cpy r7, r1
108bb448  0280a0e1  cpy r8, r2
108bb44c  829fa0e3  mov r9, #0x208
108bb450  31c1feeb  bl 0x1086b91c
108bb454  080050e1  cmp r0, r8
108bb458  2f00001a  bne 0x108bb51c
108bb45c  0400a0e3  mov r0, #0x4
108bb460  2dc1feeb  bl 0x1086b91c
108bb464  ff0000e2  and r0, r0, #0xff
108bb468  0040a0e1  cpy r4, r0
108bb46c  0010a0e1  cpy r1, r0
108bb470  0700a0e1  cpy r0, r7
108bb474  0964feeb  bl 0x108544a0
108bb478  00b0a0e1  cpy r11, r0
108bb47c  0400a0e1  cpy r0, r4
108bb480  0160a0e3  mov r6, #0x1
108bb484  cd42feeb  bl 0x1084bfc0
108bb488  00a0a0e1  cpy r10, r0
108bb48c  0040a0e3  mov r4, #0x0
108bb490  ff5004e2  and r5, r4, #0xff
108bb494  0520a0e1  cpy r2, r5
108bb498  0a10a0e1  cpy r1, r10
108bb49c  0700a0e1  cpy r0, r7
108bb4a0  8dc6ffeb  bl 0x108acedc
108bb4a4  000050e3  cmp r0, #0x0
108bb4a8  00f020e3  nop
108bb4ac  0a00000a  beq 0x108bb4dc
108bb4b0  0500a0e1  cpy r0, r5
108bb4b4  cb3ffeeb  bl 0x1084b3e8
108bb4b8  000050e3  cmp r0, #0x0
108bb4bc  00f020e3  nop
108bb4c0  0500000a  beq 0x108bb4dc
108bb4c4  000056e3  cmp r6, #0x0
108bb4c8  0100001a  bne 0x108bb4d4
108bb4cc  100055e3  cmp r5, #0x10
108bb4d0  0100000a  beq 0x108bb4dc
108bb4d4  0140a0e3  mov r4, #0x1
108bb4d8  030000ea  b 0x108bb4ec
108bb4dc  014084e2  add r4, r4, #0x1
108bb4e0  110054e3  cmp r4, #0x11
108bb4e4  e9ffff3a  bcc 0x108bb490
108bb4e8  0040a0e3  mov r4, #0x0
108bb4ec  0710a0e3  mov r1, #0x7
108bb4f0  0b00a0e1  cpy r0, r11
108bb4f4  c744feeb  bl 0x1084c818
108bb4f8  040090e1  orrs r0, r0, r4
108bb4fc  00f020e3  nop
108bb500  0500000a  beq 0x108bb51c
108bb504  0930a0e1  cpy r3, r9
108bb508  0b20a0e1  cpy r2, r11
108bb50c  0810a0e1  cpy r1, r8
108bb510  0700a0e1  cpy r0, r7
108bb514  f05fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108bb518  f33efeea  b 0x1084b0ec
108bb51c  f09fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,pc}
