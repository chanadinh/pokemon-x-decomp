108ac8fc  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108ac900  0240a0e1  cpy r4, r2
108ac904  0170a0e1  cpy r7, r1
108ac908  0200a0e3  mov r0, #0x2
108ac90c  02fcfeeb  bl 0x1086b91c
108ac910  ff6000e2  and r6, r0, #0xff
108ac914  0600a0e1  cpy r0, r6
108ac918  a87dfeeb  bl 0x1084bfc0
108ac91c  040050e1  cmp r0, r4
108ac920  3e00001a  bne 0x108aca20
108ac924  0610a0e1  cpy r1, r6
108ac928  0700a0e1  cpy r0, r7
108ac92c  db9efeeb  bl 0x108544a0
108ac930  0080a0e1  cpy r8, r0
108ac934  1610ffeb  bl 0x10870994
108ac938  0010a0e1  cpy r1, r0
108ac93c  0500a0e3  mov r0, #0x5
108ac940  ee0fffeb  bl 0x10870900
108ac944  0e0050e3  cmp r0, #0xe
108ac948  00f19f37  ldrcc pc, [pc,r0,lsl #0x2]
108ac94c  0d0000ea  b 0x108ac988
108ac950  88c98a10  .word 0x108ac988
108ac954  b0c98a10  .word 0x108ac9b0
108ac958  b0c98a10  .word 0x108ac9b0
108ac95c  a8c98a10  .word 0x108ac9a8
108ac960  a0c98a10  .word 0x108ac9a0
108ac964  98c98a10  .word 0x108ac998
108ac968  90c98a10  .word 0x108ac990
108ac96c  88c98a10  .word 0x108ac988
108ac970  b8c98a10  .word 0x108ac9b8
108ac974  c0c98a10  .word 0x108ac9c0
108ac978  c8c98a10  .word 0x108ac9c8
108ac97c  c8c98a10  .word 0x108ac9c8
108ac980  c8c98a10  .word 0x108ac9c8
108ac984  c8c98a10  .word 0x108ac9c8
108ac988  0840a0e3  mov r4, #0x8
108ac98c  0e0000ea  b 0x108ac9cc
108ac990  1040a0e3  mov r4, #0x10
108ac994  0c0000ea  b 0x108ac9cc
108ac998  2040a0e3  mov r4, #0x20
108ac99c  0a0000ea  b 0x108ac9cc
108ac9a0  4040a0e3  mov r4, #0x40
108ac9a4  080000ea  b 0x108ac9cc
108ac9a8  8040a0e3  mov r4, #0x80
108ac9ac  060000ea  b 0x108ac9cc
108ac9b0  014ca0e3  mov r4, #0x100
108ac9b4  040000ea  b 0x108ac9cc
108ac9b8  0440a0e3  mov r4, #0x4
108ac9bc  020000ea  b 0x108ac9cc
108ac9c0  0240a0e3  mov r4, #0x2
108ac9c4  000000ea  b 0x108ac9cc
108ac9c8  0140a0e3  mov r4, #0x1
108ac9cc  1f20a0e3  mov r2, #0x1f
108ac9d0  0610a0e3  mov r1, #0x6
108ac9d4  0700a0e1  cpy r0, r7
108ac9d8  67f5feeb  bl 0x10869f7c
108ac9dc  0050a0e1  cpy r5, r0
108ac9e0  0660c0e5  strb r6, [r0,#0x6]
108ac9e4  0410a0e1  cpy r1, r4
108ac9e8  0800a0e1  cpy r0, r8
108ac9ec  4082feeb  bl 0x1084d2f4
108ac9f0  b400c5e1  strh r0, [r5,#0x4]
108ac9f4  28209fe5  ldr r2, [0x108aca24]
108ac9f8  0210a0e3  mov r1, #0x2
108ac9fc  0c0085e2  add r0, r5, #0xc
108aca00  3382feeb  bl 0x1084d2d4
108aca04  0610a0e1  cpy r1, r6
108aca08  0c0085e2  add r0, r5, #0xc
108aca0c  2282feeb  bl 0x1084d29c
108aca10  0510a0e1  cpy r1, r5
108aca14  0700a0e1  cpy r0, r7
108aca18  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108aca1c  49ecfeea  b 0x10867b48
108aca20  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
