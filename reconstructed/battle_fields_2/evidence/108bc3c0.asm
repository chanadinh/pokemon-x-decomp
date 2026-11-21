108bc3c0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bc3c4  0150a0e1  cpy r5, r1
108bc3c8  0400a0e3  mov r0, #0x4
108bc3cc  52bdfeeb  bl 0x1086b91c
108bc3d0  ff4000e2  and r4, r0, #0xff
108bc3d4  0410a0e1  cpy r1, r4
108bc3d8  0500a0e1  cpy r0, r5
108bc3dc  d03efeeb  bl 0x1084bf24
108bc3e0  000050e3  cmp r0, #0x0
108bc3e4  1a00001a  bne 0x108bc454
108bc3e8  0410a0e1  cpy r1, r4
108bc3ec  0500a0e1  cpy r0, r5
108bc3f0  ce3afeeb  bl 0x1084af30
108bc3f4  000050e3  cmp r0, #0x0
108bc3f8  1500001a  bne 0x108bc454
108bc3fc  1e00a0e3  mov r0, #0x1e
108bc400  45bdfeeb  bl 0x1086b91c
108bc404  7000ffe6  uxth r0, r0
108bc408  bd3efeeb  bl 0x1084bf04
108bc40c  000050e3  cmp r0, #0x0
108bc410  0f00000a  beq 0x108bc454
108bc414  0110a0e3  mov r1, #0x1
108bc418  4200a0e3  mov r0, #0x42
108bc41c  2ca5feeb  bl 0x108658d4
108bc420  000050e3  cmp r0, #0x0
108bc424  0a00000a  beq 0x108bc454
108bc428  0420a0e1  cpy r2, r4
108bc42c  1710a0e3  mov r1, #0x17
108bc430  0500a0e1  cpy r0, r5
108bc434  d0b6feeb  bl 0x10869f7c
108bc438  0010a0e1  cpy r1, r0
108bc43c  1100a0e3  mov r0, #0x11
108bc440  0540c1e5  strb r4, [r1,#0x5]
108bc444  0400c1e5  strb r0, [r1,#0x4]
108bc448  0500a0e1  cpy r0, r5
108bc44c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bc450  bcadfeea  b 0x10867b48
108bc454  7080bde8  ldmia sp!,{r4,r5,r6,pc}
