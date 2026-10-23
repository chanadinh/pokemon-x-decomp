003e0aa8  010070e3  cmn r0,#0x1
003e0aac  201ca0e1  mov r1,r0, lsr #0x18
003e0ab0  0020a0e1  cpy r2,r0
003e0ab4  ff005113  cmpne r1,#0xff
003e0ab8  0204a011  movne r0,r2, lsl #0x8
003e0abc  10402de9  stmdb sp!,{r4,lr}
003e0ac0  200ca011  movne r0,r0, lsr #0x18
003e0ac4  0400001a  bne 0x003e0adc
003e0ac8  0020a0e3  mov r2,#0x0
003e0acc  0210a0e1  cpy r1,r2
003e0ad0  0200a0e1  cpy r0,r2
003e0ad4  0000a0e1  cpy r0,r0
003e0ad8  0000a0e3  mov r0,#0x0
003e0adc  1080bde8  ldmia sp!,{r4,pc}
