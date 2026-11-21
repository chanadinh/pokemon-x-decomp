108b4ef8  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b4efc  0160a0e1  cpy r6, r1
108b4f00  0250a0e1  cpy r5, r2
108b4f04  0300a0e3  mov r0, #0x3
108b4f08  83dafeeb  bl 0x1086b91c
108b4f0c  050050e1  cmp r0, r5
108b4f10  5600001a  bne 0x108b5070
108b4f14  0510a0e1  cpy r1, r5
108b4f18  0600a0e1  cpy r0, r6
108b4f1c  5f7dfeeb  bl 0x108544a0
108b4f20  0040a0e1  cpy r4, r0
108b4f24  1210a0e3  mov r1, #0x12
108b4f28  d686feeb  bl 0x10856a88
108b4f2c  000050e3  cmp r0, #0x0
108b4f30  0170a0e3  mov r7, #0x1
108b4f34  0a00000a  beq 0x108b4f64
108b4f38  0520a0e1  cpy r2, r5
108b4f3c  0a10a0e3  mov r1, #0xa
108b4f40  0600a0e1  cpy r0, r6
108b4f44  0cd4feeb  bl 0x10869f7c
108b4f48  0010a0e1  cpy r1, r0
108b4f4c  1200a0e3  mov r0, #0x12
108b4f50  1170c1e5  strb r7, [r1,#0x11]
108b4f54  0550c1e5  strb r5, [r1,#0x5]
108b4f58  0400c1e5  strb r0, [r1,#0x4]
108b4f5c  0600a0e1  cpy r0, r6
108b4f60  f8cafeeb  bl 0x10867b48
108b4f64  0810a0e3  mov r1, #0x8
108b4f68  0400a0e1  cpy r0, r4
108b4f6c  c586feeb  bl 0x10856a88
108b4f70  000050e3  cmp r0, #0x0
108b4f74  00f020e3  nop
108b4f78  0a00000a  beq 0x108b4fa8
108b4f7c  0520a0e1  cpy r2, r5
108b4f80  0a10a0e3  mov r1, #0xa
108b4f84  0600a0e1  cpy r0, r6
108b4f88  fbd3feeb  bl 0x10869f7c
108b4f8c  1170c0e5  strb r7, [r0,#0x11]
108b4f90  0810a0e3  mov r1, #0x8
108b4f94  0550c0e5  strb r5, [r0,#0x5]
108b4f98  0410c0e5  strb r1, [r0,#0x4]
108b4f9c  0010a0e1  cpy r1, r0
108b4fa0  0600a0e1  cpy r0, r6
108b4fa4  e7cafeeb  bl 0x10867b48
108b4fa8  0520a0e1  cpy r2, r5
108b4fac  1a10a0e3  mov r1, #0x1a
108b4fb0  0600a0e1  cpy r0, r6
108b4fb4  f0d3feeb  bl 0x10869f7c
108b4fb8  0040a0e1  cpy r4, r0
108b4fbc  0500a0e1  cpy r0, r5
108b4fc0  fe5bfeeb  bl 0x1084bfc0
108b4fc4  0800c4e5  strb r0, [r4,#0x8]
108b4fc8  0410a0e3  mov r1, #0x4
108b4fcc  0410c4e5  strb r1, [r4,#0x4]
108b4fd0  0010a0e3  mov r1, #0x0
108b4fd4  050084e2  add r0, r4, #0x5
108b4fd8  0120a0e3  mov r2, #0x1
108b4fdc  0510c4e5  strb r1, [r4,#0x5]
108b4fe0  0110c0e5  strb r1, [r0,#0x1]
108b4fe4  012052e2  subs r2, r2, #0x1
108b4fe8  0210e0e5  strb r1, [r0,#0x2]!
108b4fec  fbffff1a  bne 0x108b4fe0
108b4ff0  0410d4e5  ldrb r1, [r4,#0x4]
108b4ff4  040084e2  add r0, r4, #0x4
108b4ff8  010051e3  cmp r1, #0x1
108b4ffc  0200009a  bls 0x108b500c
108b5000  0110d0e5  ldrb r1, [r0,#0x1]
108b5004  401081e3  orr r1, r1, #0x40
108b5008  0110c0e5  strb r1, [r0,#0x1]
108b500c  0410d4e5  ldrb r1, [r4,#0x4]
108b5010  040084e2  add r0, r4, #0x4
108b5014  010051e3  cmp r1, #0x1
108b5018  0200009a  bls 0x108b5028
108b501c  0110d0e5  ldrb r1, [r0,#0x1]
108b5020  801081e3  orr r1, r1, #0x80
108b5024  0110c0e5  strb r1, [r0,#0x1]
108b5028  0410d4e5  ldrb r1, [r4,#0x4]
108b502c  040084e2  add r0, r4, #0x4
108b5030  020051e3  cmp r1, #0x2
108b5034  0200009a  bls 0x108b5044
108b5038  0210d0e5  ldrb r1, [r0,#0x2]
108b503c  011081e3  orr r1, r1, #0x1
108b5040  0210c0e5  strb r1, [r0,#0x2]
108b5044  0410d4e5  ldrb r1, [r4,#0x4]
108b5048  040084e2  add r0, r4, #0x4
108b504c  020051e3  cmp r1, #0x2
108b5050  0200009a  bls 0x108b5060
108b5054  0210d0e5  ldrb r1, [r0,#0x2]
108b5058  401081e3  orr r1, r1, #0x40
108b505c  0210c0e5  strb r1, [r0,#0x2]
108b5060  0410a0e1  cpy r1, r4
108b5064  0600a0e1  cpy r0, r6
108b5068  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108b506c  b5cafeea  b 0x10867b48
108b5070  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
