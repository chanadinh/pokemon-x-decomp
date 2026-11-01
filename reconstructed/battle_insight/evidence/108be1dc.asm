108be1dc  f04f2de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11,lr}
108be1e0  0cd04de2  sub sp, sp, #0xc
108be1e4  0280a0e1  cpy r8, r2
108be1e8  0190a0e1  cpy r9, r1
108be1ec  0200a0e3  mov r0, #0x2
108be1f0  c9b5feeb  bl 0x1086b91c
108be1f4  080050e1  cmp r0, r8
108be1f8  2e00001a  bne 0x108be2b8
108be1fc  0d20a0e1  cpy r2, sp
108be200  0810a0e1  cpy r1, r8
108be204  0900a0e1  cpy r0, r9
108be208  8332feeb  bl 0x1084ac1c
108be20c  007050e2  subs r7, r0, #0x0
108be210  2800000a  beq 0x108be2b8
108be214  0050a0e3  mov r5, #0x0
108be218  0560a0e1  cpy r6, r5
108be21c  0540a0e1  cpy r4, r5
108be220  0db0a081  cpyhi r11, sp
108be224  1300009a  bls 0x108be278
108be228  0410dbe7  ldrb r1, [r11,r4]
108be22c  0900a0e1  cpy r0, r9
108be230  9a58feeb  bl 0x108544a0
108be234  00a0a0e1  cpy r10, r0
108be238  0910a0e3  mov r1, #0x9
108be23c  77d8feeb  bl 0x10874420
108be240  050080e0  add r0, r0, r5
108be244  0b10a0e3  mov r1, #0xb
108be248  7050ffe6  uxth r5, r0
108be24c  0a00a0e1  cpy r0, r10
108be250  72d8feeb  bl 0x10874420
108be254  011084e2  add r1, r4, #0x1
108be258  060080e0  add r0, r0, r6
108be25c  ff4001e2  and r4, r1, #0xff
108be260  7060ffe6  uxth r6, r0
108be264  070054e1  cmp r4, r7
108be268  eeffff3a  bcc 0x108be228
108be26c  060055e1  cmp r5, r6
108be270  0140a033  movcc r4, #0x1
108be274  0000003a  bcc 0x108be27c
108be278  0340a0e3  mov r4, #0x3
108be27c  0820a0e1  cpy r2, r8
108be280  0d10a0e3  mov r1, #0xd
108be284  0900a0e1  cpy r0, r9
108be288  3baffeeb  bl 0x10869f7c
108be28c  0010a0e1  cpy r1, r0
108be290  000090e5  ldr r0, [r0,#0x0]
108be294  0c80c1e5  strb r8, [r1,#0xc]
108be298  0120a0e3  mov r2, #0x1
108be29c  0440c1e5  strb r4, [r1,#0x4]
108be2a0  020580e3  orr r0, r0, #0x800000
108be2a4  1620c1e5  strb r2, [r1,#0x16]
108be2a8  000081e5  str r0, [r1,#0x0]
108be2ac  0900a0e1  cpy r0, r9
108be2b0  1720c1e5  strb r2, [r1,#0x17]
108be2b4  23a6feeb  bl 0x10867b48
108be2b8  0cd08de2  add sp, sp, #0xc
108be2bc  f08fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,pc}
