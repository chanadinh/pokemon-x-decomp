108bb688  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108bb68c  0300a0e3  mov r0, #0x3
108bb690  0170a0e1  cpy r7, r1
108bb694  0280a0e1  cpy r8, r2
108bb698  9fc0feeb  bl 0x1086b91c
108bb69c  080050e1  cmp r0, r8
108bb6a0  2f00001a  bne 0x108bb764
108bb6a4  0600a0e3  mov r0, #0x6
108bb6a8  9bc0feeb  bl 0x1086b91c
108bb6ac  ff0000e2  and r0, r0, #0xff
108bb6b0  0040a0e1  cpy r4, r0
108bb6b4  0010a0e1  cpy r1, r0
108bb6b8  0700a0e1  cpy r0, r7
108bb6bc  7763feeb  bl 0x108544a0
108bb6c0  00a0a0e1  cpy r10, r0
108bb6c4  0400a0e1  cpy r0, r4
108bb6c8  0160a0e3  mov r6, #0x1
108bb6cc  3b42feeb  bl 0x1084bfc0
108bb6d0  0090a0e1  cpy r9, r0
108bb6d4  0040a0e3  mov r4, #0x0
108bb6d8  ff5004e2  and r5, r4, #0xff
108bb6dc  0520a0e1  cpy r2, r5
108bb6e0  0910a0e1  cpy r1, r9
108bb6e4  0700a0e1  cpy r0, r7
108bb6e8  fbc5ffeb  bl 0x108acedc
108bb6ec  000050e3  cmp r0, #0x0
108bb6f0  00f020e3  nop
108bb6f4  0a00000a  beq 0x108bb724
108bb6f8  0500a0e1  cpy r0, r5
108bb6fc  393ffeeb  bl 0x1084b3e8
108bb700  000050e3  cmp r0, #0x0
108bb704  00f020e3  nop
108bb708  0500000a  beq 0x108bb724
108bb70c  000056e3  cmp r6, #0x0
108bb710  0100001a  bne 0x108bb71c
108bb714  100055e3  cmp r5, #0x10
108bb718  0100000a  beq 0x108bb724
108bb71c  0140a0e3  mov r4, #0x1
108bb720  030000ea  b 0x108bb734
108bb724  014084e2  add r4, r4, #0x1
108bb728  110054e3  cmp r4, #0x11
108bb72c  e9ffff3a  bcc 0x108bb6d8
108bb730  0040a0e3  mov r4, #0x0
108bb734  0710a0e3  mov r1, #0x7
108bb738  0a00a0e1  cpy r0, r10
108bb73c  3544feeb  bl 0x1084c818
108bb740  040090e1  orrs r0, r0, r4
108bb744  00f020e3  nop
108bb748  0500000a  beq 0x108bb764
108bb74c  0a20a0e1  cpy r2, r10
108bb750  0810a0e1  cpy r1, r8
108bb754  0700a0e1  cpy r0, r7
108bb758  f047bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108bb75c  823fa0e3  mov r3, #0x208
108bb760  613efeea  b 0x1084b0ec
108bb764  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
