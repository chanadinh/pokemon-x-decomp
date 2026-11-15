108ba8b4  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108ba8b8  0260a0e1  cpy r6, r2
108ba8bc  0170a0e1  cpy r7, r1
108ba8c0  0350a0e1  cpy r5, r3
108ba8c4  0300a0e3  mov r0, #0x3
108ba8c8  13c4feeb  bl 0x1086b91c
108ba8cc  060050e1  cmp r0, r6
108ba8d0  1b00001a  bne 0x108ba944
108ba8d4  0610a0e1  cpy r1, r6
108ba8d8  0700a0e1  cpy r0, r7
108ba8dc  ef66feeb  bl 0x108544a0
108ba8e0  0410a0e3  mov r1, #0x4
108ba8e4  9044feeb  bl 0x1084bb2c
108ba8e8  000050e3  cmp r0, #0x0
108ba8ec  0900000a  beq 0x108ba918
108ba8f0  010040e2  sub r0, r0, #0x1
108ba8f4  ff4000e2  and r4, r0, #0xff
108ba8f8  180054e3  cmp r4, #0x18
108ba8fc  0500002a  bcs 0x108ba918
108ba900  0410a0e1  cpy r1, r4
108ba904  0700a0e1  cpy r0, r7
108ba908  9a48feeb  bl 0x1084cb78
108ba90c  0a0050e3  cmp r0, #0xa
108ba910  0400001a  bne 0x108ba928
108ba914  000000ea  b 0x108ba91c
108ba918  1f40a0e3  mov r4, #0x1f
108ba91c  0110a0e3  mov r1, #0x1
108ba920  4200a0e3  mov r0, #0x42
108ba924  eaabfeeb  bl 0x108658d4
108ba928  0030a0e3  mov r3, #0x0
108ba92c  0320a0e1  cpy r2, r3
108ba930  0610a0e1  cpy r1, r6
108ba934  0700a0e1  cpy r0, r7
108ba938  a9b6feeb  bl 0x108683e4
108ba93c  0100a0e3  mov r0, #0x1
108ba940  110085e8  stmia r5,{r0,r4}
108ba944  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
