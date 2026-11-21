108b898c  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b8990  0160a0e1  cpy r6, r1
108b8994  0070a0e1  cpy r7, r0
108b8998  0240a0e1  cpy r4, r2
108b899c  0200a0e3  mov r0, #0x2
108b89a0  ddcbfeeb  bl 0x1086b91c
108b89a4  040050e1  cmp r0, r4
108b89a8  2f00001a  bne 0x108b8a6c
108b89ac  0410a0e1  cpy r1, r4
108b89b0  0600a0e1  cpy r0, r6
108b89b4  b96efeeb  bl 0x108544a0
108b89b8  0050a0e1  cpy r5, r0
108b89bc  0210a0e3  mov r1, #0x2
108b89c0  3078feeb  bl 0x10856a88
108b89c4  000050e3  cmp r0, #0x0
108b89c8  2700000a  beq 0x108b8a6c
108b89cc  0420a0e1  cpy r2, r4
108b89d0  0a10a0e3  mov r1, #0xa
108b89d4  0600a0e1  cpy r0, r6
108b89d8  67c5feeb  bl 0x10869f7c
108b89dc  0010a0e1  cpy r1, r0
108b89e0  1920a0e3  mov r2, #0x19
108b89e4  0420c1e5  strb r2, [r1,#0x4]
108b89e8  0100a0e3  mov r0, #0x1
108b89ec  0540c1e5  strb r4, [r1,#0x5]
108b89f0  1100c1e5  strb r0, [r1,#0x11]
108b89f4  0600a0e1  cpy r0, r6
108b89f8  52bcfeeb  bl 0x10867b48
108b89fc  0540a0e1  cpy r4, r5
108b8a00  0700a0e1  cpy r0, r7
108b8a04  b803d0e1  ldrh r0, [r0,#0x38]
108b8a08  0050a0e1  cpy r5, r0
108b8a0c  0400a0e1  cpy r0, r4
108b8a10  1900d0e5  ldrb r0, [r0,#0x19]
108b8a14  0060a0e1  cpy r6, r0
108b8a18  0010a0e1  cpy r1, r0
108b8a1c  0000a0e3  mov r0, #0x0
108b8a20  6652feeb  bl 0x1084d3c0
108b8a24  0040b0e1  movs r4, r0
108b8a28  0f00000a  beq 0x108b8a6c
108b8a2c  00f020e3  nop
108b8a30  b803d0e1  ldrh r0, [r0,#0x38]
108b8a34  050050e1  cmp r0, r5
108b8a38  0400a0e1  cpy r0, r4
108b8a3c  0600001a  bne 0x108b8a5c
108b8a40  e151feeb  bl 0x1084d1cc
108b8a44  0610a0e1  cpy r1, r6
108b8a48  0000a0e3  mov r0, #0x0
108b8a4c  5b52feeb  bl 0x1084d3c0
108b8a50  00f020e3  nop
108b8a54  00f020e3  nop
108b8a58  010000ea  b 0x108b8a64
108b8a5c  00f020e3  nop
108b8a60  4252feeb  bl 0x1084d370
108b8a64  0040b0e1  movs r4, r0
108b8a68  efffff1a  bne 0x108b8a2c
108b8a6c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
