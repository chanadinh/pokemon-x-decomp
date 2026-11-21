108ac704  70402de9  stmdb sp!,{r4,r5,r6,lr}
108ac708  0240a0e1  cpy r4, r2
108ac70c  0400a0e3  mov r0, #0x4
108ac710  81fcfeeb  bl 0x1086b91c
108ac714  ff5000e2  and r5, r0, #0xff
108ac718  0500a0e1  cpy r0, r5
108ac71c  277efeeb  bl 0x1084bfc0
108ac720  040050e1  cmp r0, r4
108ac724  1c00001a  bne 0x108ac79c
108ac728  1200a0e3  mov r0, #0x12
108ac72c  7afcfeeb  bl 0x1086b91c
108ac730  7060ffe6  uxth r6, r0
108ac734  1b10a0e3  mov r1, #0x1b
108ac738  0600a0e1  cpy r0, r6
108ac73c  544ffdeb  bl 0x10800494
108ac740  ff4000e2  and r4, r0, #0xff
108ac744  0600a0e1  cpy r0, r6
108ac748  394ffdeb  bl 0x10800434
108ac74c  000050e3  cmp r0, #0x0
108ac750  1100000a  beq 0x108ac79c
108ac754  050054e3  cmp r4, #0x5
108ac758  04005413  cmpne r4, #0x4
108ac75c  0e00001a  bne 0x108ac79c
108ac760  0110a0e3  mov r1, #0x1
108ac764  4100a0e3  mov r0, #0x41
108ac768  59e4feeb  bl 0x108658d4
108ac76c  000050e3  cmp r0, #0x0
108ac770  0900000a  beq 0x108ac79c
108ac774  4000a0e3  mov r0, #0x40
108ac778  67fcfeeb  bl 0x1086b91c
108ac77c  1c209fe5  ldr r2, [0x108ac7a0]
108ac780  0040a0e1  cpy r4, r0
108ac784  0210a0e3  mov r1, #0x2
108ac788  d182feeb  bl 0x1084d2d4
108ac78c  0510a0e1  cpy r1, r5
108ac790  0400a0e1  cpy r0, r4
108ac794  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108ac798  bf82feea  b 0x1084d29c
108ac79c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
