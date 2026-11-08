108c2f4c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c2f50  0240a0e1  cpy r4, r2
108c2f54  0350a0e1  cpy r5, r3
108c2f58  0200a0e3  mov r0, #0x2
108c2f5c  6ea2feeb  bl 0x1086b91c
108c2f60  040050e1  cmp r0, r4
108c2f64  0f00001a  bne 0x108c2fa8
108c2f68  1200a0e3  mov r0, #0x12
108c2f6c  6aa2feeb  bl 0x1086b91c
108c2f70  7000ffe6  uxth r0, r0
108c2f74  0020a0e3  mov r2, #0x0
108c2f78  011c40e2  sub r1, r0, #0x100
108c2f7c  6b1051e2  subs r1, r1, #0x6b
108c2f80  082085e5  str r2, [r5,#0x8]
108c2f84  0700001a  bne 0x108c2fa8
108c2f88  0200a0e3  mov r0, #0x2
108c2f8c  62a2feeb  bl 0x1086b91c
108c2f90  040050e1  cmp r0, r4
108c2f94  0300001a  bne 0x108c2fa8
108c2f98  1310a0e3  mov r1, #0x13
108c2f9c  2300a0e3  mov r0, #0x23
108c2fa0  4b8afeeb  bl 0x108658d4
108c2fa4  080085e5  str r0, [r5,#0x8]
108c2fa8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
