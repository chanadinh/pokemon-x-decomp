10871250  10402de9  stmdb sp!,{r4,lr}
10871254  0030a0e1  cpy r3, r0
10871258  0240a0e1  cpy r4, r2
1087125c  9cc3ffeb  bl 0x108620d4
10871260  080040e2  sub r0, r0, #0x8
10871264  050050e3  cmp r0, #0x5
10871268  00f19f37  ldrcc pc, [pc,r0,lsl #0x2]
1087126c  0e0000ea  b 0x108712ac
10871284  b64fc3e1  strh r4, [r3,#0xf6]
10871288  1080bde8  ldmia sp!,{r4,pc}
1087128c  b84fc3e1  strh r4, [r3,#0xf8]
10871290  1080bde8  ldmia sp!,{r4,pc}
10871294  ba4fc3e1  strh r4, [r3,#0xfa]
10871298  1080bde8  ldmia sp!,{r4,pc}
1087129c  bc4fc3e1  strh r4, [r3,#0xfc]
108712a0  1080bde8  ldmia sp!,{r4,pc}
108712a4  be4fc3e1  strh r4, [r3,#0xfe]
108712a8  1080bde8  ldmia sp!,{r4,pc}
108712ac  0020a0e3  mov r2, #0x0
108712b0  1040bde8  ldmia sp!,{r4,lr}
108712b4  0210a0e1  cpy r1, r2
108712b8  0200a0e1  cpy r0, r2
108712bc  343cfeea  b 0x10800394
