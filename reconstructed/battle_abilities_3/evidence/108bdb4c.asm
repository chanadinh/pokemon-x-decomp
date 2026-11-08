108bdb4c  f05f2de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108bdb50  0200a0e3  mov r0, #0x2
108bdb54  01a0a0e1  cpy r10, r1
108bdb58  0290a0e1  cpy r9, r2
108bdb5c  6eb7feeb  bl 0x1086b91c
108bdb60  090050e1  cmp r0, r9
108bdb64  8900001a  bne 0x108bdd90
108bdb68  b610a0e3  mov r1, #0xb6
108bdb6c  0a00a0e1  cpy r0, r10
108bdb70  163bfeeb  bl 0x1084c7d0
108bdb74  0040a0e1  cpy r4, r0
108bdb78  0910a0e1  cpy r1, r9
108bdb7c  0a00a0e1  cpy r0, r10
108bdb80  465afeeb  bl 0x108544a0
108bdb84  0050a0e3  mov r5, #0x0
108bdb88  00b0a0e1  cpy r11, r0
108bdb8c  0580a0e1  cpy r8, r5
108bdb90  0560a0e1  cpy r6, r5
108bdb94  017086e2  add r7, r6, #0x1
108bdb98  ff1007e2  and r1, r7, #0xff
108bdb9c  0120a0e3  mov r2, #0x1
108bdba0  0b00a0e1  cpy r0, r11
108bdba4  cfd1feeb  bl 0x108722e8
108bdba8  000050e3  cmp r0, #0x0
108bdbac  00f020e3  nop
108bdbb0  0300000a  beq 0x108bdbc4
108bdbb4  081084e0  add r1, r4, r8
108bdbb8  010088e2  add r0, r8, #0x1
108bdbbc  ff8000e2  and r8, r0, #0xff
108bdbc0  a870c1e5  strb r7, [r1,#0xa8]
108bdbc4  ff1007e2  and r1, r7, #0xff
108bdbc8  0020e0e3  mvn r2, #0x0
108bdbcc  0b00a0e1  cpy r0, r11
108bdbd0  c4d1feeb  bl 0x108722e8
108bdbd4  000050e3  cmp r0, #0x0
108bdbd8  00f020e3  nop
108bdbdc  0300000a  beq 0x108bdbf0
108bdbe0  051084e0  add r1, r4, r5
108bdbe4  010085e2  add r0, r5, #0x1
108bdbe8  ff5000e2  and r5, r0, #0xff
108bdbec  af70c1e5  strb r7, [r1,#0xaf]
108bdbf0  016086e2  add r6, r6, #0x1
108bdbf4  070056e3  cmp r6, #0x7
108bdbf8  e5ffff3a  bcc 0x108bdb94
108bdbfc  000058e3  cmp r8, #0x0
108bdc00  0170a0e3  mov r7, #0x1
108bdc04  0060a0e3  mov r6, #0x0
108bdc08  4700000a  beq 0x108bdd2c
108bdc0c  000055e3  cmp r5, #0x0
108bdc10  5900000a  beq 0x108bdd7c
108bdc14  0000a0e3  mov r0, #0x0
108bdc18  000058e3  cmp r8, #0x0
108bdc1c  0020a0e1  cpy r2, r0
108bdc20  1500009a  bls 0x108bdc7c
108bdc24  000055e3  cmp r5, #0x0
108bdc28  0010a0e3  mov r1, #0x0
108bdc2c  02608480  addhi r6, r4, r2
108bdc30  0e00009a  bls 0x108bdc70
108bdc34  01c084e0  add r12, r4, r1
108bdc38  a830d6e5  ldrb r3, [r6,#0xa8]
108bdc3c  afc0dce5  ldrb r12, [r12,#0xaf]
108bdc40  0c0053e1  cmp r3, r12
108bdc44  0600000a  beq 0x108bdc64
108bdc48  03348ce1  orr r3, r12, r3, lsl #0x8
108bdc4c  80c084e0  add r12, r4, r0, lsl #0x1
108bdc50  01b080e2  add r11, r0, #0x1
108bdc54  ff000be2  and r0, r11, #0xff
108bdc58  540050e3  cmp r0, #0x54
108bdc5c  b030cce1  strh r3, [r12,#0x0]
108bdc60  0200002a  bcs 0x108bdc70
108bdc64  011081e2  add r1, r1, #0x1
108bdc68  050051e1  cmp r1, r5
108bdc6c  f0ffff3a  bcc 0x108bdc34
108bdc70  012082e2  add r2, r2, #0x1
108bdc74  020058e1  cmp r8, r2
108bdc78  e9ffff8a  bhi 0x108bdc24
108bdc7c  00f020e3  nop
108bdc80  cd3afeeb  bl 0x1084c7bc
108bdc84  800084e0  add r0, r4, r0, lsl #0x1
108bdc88  b000d0e1  ldrh r0, [r0,#0x0]
108bdc8c  ff6000e2  and r6, r0, #0xff
108bdc90  2044a0e1  mov r4, r0, lsr #0x8
108bdc94  0920a0e1  cpy r2, r9
108bdc98  0110a0e3  mov r1, #0x1
108bdc9c  0a00a0e1  cpy r0, r10
108bdca0  283bfeeb  bl 0x1084c948
108bdca4  000054e3  cmp r4, #0x0
108bdca8  00f020e3  nop
108bdcac  0b00000a  beq 0x108bdce0
108bdcb0  0920a0e1  cpy r2, r9
108bdcb4  0d10a0e3  mov r1, #0xd
108bdcb8  0a00a0e1  cpy r0, r10
108bdcbc  aeb0feeb  bl 0x10869f7c
108bdcc0  0010a0e1  cpy r1, r0
108bdcc4  0200a0e3  mov r0, #0x2
108bdcc8  1670c1e5  strb r7, [r1,#0x16]
108bdccc  0c90c1e5  strb r9, [r1,#0xc]
108bdcd0  1700c1e5  strb r0, [r1,#0x17]
108bdcd4  0a00a0e1  cpy r0, r10
108bdcd8  0440c1e5  strb r4, [r1,#0x4]
108bdcdc  99a7feeb  bl 0x10867b48
108bdce0  000056e3  cmp r6, #0x0
108bdce4  0b00000a  beq 0x108bdd18
108bdce8  0920a0e1  cpy r2, r9
108bdcec  0d10a0e3  mov r1, #0xd
108bdcf0  0a00a0e1  cpy r0, r10
108bdcf4  a0b0feeb  bl 0x10869f7c
108bdcf8  0010a0e1  cpy r1, r0
108bdcfc  ff00a0e3  mov r0, #0xff
108bdd00  1670c1e5  strb r7, [r1,#0x16]
108bdd04  0c90c1e5  strb r9, [r1,#0xc]
108bdd08  1700c1e5  strb r0, [r1,#0x17]
108bdd0c  0a00a0e1  cpy r0, r10
108bdd10  0460c1e5  strb r6, [r1,#0x4]
108bdd14  8ba7feeb  bl 0x10867b48
108bdd18  0920a0e1  cpy r2, r9
108bdd1c  0a00a0e1  cpy r0, r10
108bdd20  f05fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108bdd24  0210a0e3  mov r1, #0x2
108bdd28  063bfeea  b 0x1084c948
108bdd2c  000055e3  cmp r5, #0x0
108bdd30  0a00000a  beq 0x108bdd60
108bdd34  0500a0e1  cpy r0, r5
108bdd38  9f3afeeb  bl 0x1084c7bc
108bdd3c  040080e0  add r0, r0, r4
108bdd40  0920a0e1  cpy r2, r9
108bdd44  af60d0e5  ldrb r6, [r0,#0xaf]
108bdd48  0110a0e3  mov r1, #0x1
108bdd4c  0a00a0e1  cpy r0, r10
108bdd50  fc3afeeb  bl 0x1084c948
108bdd54  00f020e3  nop
108bdd58  00f020e3  nop
108bdd5c  dfffffea  b 0x108bdce0
108bdd60  0920a0e1  cpy r2, r9
108bdd64  0110a0e3  mov r1, #0x1
108bdd68  0a00a0e1  cpy r0, r10
108bdd6c  f53afeeb  bl 0x1084c948
108bdd70  00f020e3  nop
108bdd74  00f020e3  nop
108bdd78  e6ffffea  b 0x108bdd18
108bdd7c  0800a0e1  cpy r0, r8
108bdd80  8d3afeeb  bl 0x1084c7bc
108bdd84  040080e0  add r0, r0, r4
108bdd88  a840d0e5  ldrb r4, [r0,#0xa8]
108bdd8c  c0ffffea  b 0x108bdc94
108bdd90  f09fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,pc}
