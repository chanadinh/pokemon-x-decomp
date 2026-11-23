1084bac4  030051e3  cmp r1, #0x3
1084bac8  00c0a0e1  cpy r12, r0
1084bacc  10402de9  stmdb sp!,{r4,lr}
1084bad0  1300002a  bcs 0x1084bb24
1084bad4  0502dce5  ldrb r0, [r12,#0x205]
1084bad8  010040e0  sub r0, r0, r1
1084badc  000050e3  cmp r0, #0x0
1084bae0  030080b2  addlt r0, r0, #0x3
1084bae4  00108ce0  add r1, r12, r0
1084bae8  0212d1e5  ldrb r1, [r1,#0x202]
1084baec  020051e1  cmp r1, r2
1084baf0  0b00009a  bls 0x1084bb24
1084baf4  021041e0  sub r1, r1, r2
1084baf8  802080e0  add r2, r0, r0, lsl #0x1
1084bafc  010041e2  sub r0, r1, #0x1
1084bb00  ff0000e2  and r0, r0, #0xff
1084bb04  02128ce0  add r1, r12, r2, lsl #0x4
1084bb08  800181e0  add r0, r1, r0, lsl #0x3
1084bb0c  011c80e2  add r1, r0, #0x100
1084bb10  721081e2  add r1, r1, #0x72
1084bb14  0300a0e1  cpy r0, r3
1084bb18  37a300eb  bl 0x108747fc
1084bb1c  0100a0e3  mov r0, #0x1
1084bb20  1080bde8  ldmia sp!,{r4,pc}
1084bb24  0000a0e3  mov r0, #0x0
1084bb28  1080bde8  ldmia sp!,{r4,pc}
