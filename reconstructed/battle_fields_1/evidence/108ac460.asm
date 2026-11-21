108ac460  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108ac464  0050a0e1  cpy r5, r0
108ac468  0170a0e1  cpy r7, r1
108ac46c  0240a0e1  cpy r4, r2
108ac470  0200a0e3  mov r0, #0x2
108ac474  28fdfeeb  bl 0x1086b91c
108ac478  ff6000e2  and r6, r0, #0xff
108ac47c  0600a0e1  cpy r0, r6
108ac480  ce7efeeb  bl 0x1084bfc0
108ac484  040050e1  cmp r0, r4
108ac488  3300001a  bne 0x108ac55c
108ac48c  0610a0e1  cpy r1, r6
108ac490  0700a0e1  cpy r0, r7
108ac494  a27efeeb  bl 0x1084bf24
108ac498  000050e3  cmp r0, #0x0
108ac49c  2e00001a  bne 0x108ac55c
108ac4a0  0500a0e1  cpy r0, r5
108ac4a4  b803d0e1  ldrh r0, [r0,#0x38]
108ac4a8  ff0000e2  and r0, r0, #0xff
108ac4ac  110050e3  cmp r0, #0x11
108ac4b0  02005433  cmpcc r4, #0x2
108ac4b4  0600002a  bcs 0x108ac4d4
108ac4b8  a0209fe5  ldr r2, [0x108ac560]
108ac4bc  041284e0  add r1, r4, r4, lsl #0x4
108ac4c0  011282e0  add r1, r2, r1, lsl #0x4
108ac4c4  000281e0  add r0, r1, r0, lsl #0x4
108ac4c8  0c0090e5  ldr r0, [r0,#0xc]
108ac4cc  ff0000e2  and r0, r0, #0xff
108ac4d0  040000ea  b 0x108ac4e8
108ac4d4  0020a0e3  mov r2, #0x0
108ac4d8  0210a0e1  cpy r1, r2
108ac4dc  0200a0e1  cpy r0, r2
108ac4e0  ab4ffdeb  bl 0x10800394
108ac4e4  0000a0e3  mov r0, #0x0
108ac4e8  020050e3  cmp r0, #0x2
108ac4ec  0650a003  moveq r5, #0x6
108ac4f0  0200000a  beq 0x108ac500
108ac4f4  030050e3  cmp r0, #0x3
108ac4f8  0850a013  movne r5, #0x8
108ac4fc  0450a003  moveq r5, #0x4
108ac500  1f20a0e3  mov r2, #0x1f
108ac504  0610a0e3  mov r1, #0x6
108ac508  0700a0e1  cpy r0, r7
108ac50c  9af6feeb  bl 0x10869f7c
108ac510  0040a0e1  cpy r4, r0
108ac514  0610a0e1  cpy r1, r6
108ac518  0700a0e1  cpy r0, r7
108ac51c  df9ffeeb  bl 0x108544a0
108ac520  0510a0e1  cpy r1, r5
108ac524  0660c4e5  strb r6, [r4,#0x6]
108ac528  7183feeb  bl 0x1084d2f4
108ac52c  b400c4e1  strh r0, [r4,#0x4]
108ac530  2c209fe5  ldr r2, [0x108ac564]
108ac534  0210a0e3  mov r1, #0x2
108ac538  0c0084e2  add r0, r4, #0xc
108ac53c  6483feeb  bl 0x1084d2d4
108ac540  0610a0e1  cpy r1, r6
108ac544  0c0084e2  add r0, r4, #0xc
108ac548  5383feeb  bl 0x1084d29c
108ac54c  0410a0e1  cpy r1, r4
108ac550  0700a0e1  cpy r0, r7
108ac554  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108ac558  7aedfeea  b 0x10867b48
108ac55c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
