108b1898  f05f2de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108b189c  0250a0e1  cpy r5, r2
108b18a0  0300a0e3  mov r0, #0x3
108b18a4  0190a0e1  cpy r9, r1
108b18a8  1be8feeb  bl 0x1086b91c
108b18ac  050050e1  cmp r0, r5
108b18b0  6b00001a  bne 0x108b1a64
108b18b4  0600a0e3  mov r0, #0x6
108b18b8  17e8feeb  bl 0x1086b91c
108b18bc  ff6000e2  and r6, r0, #0xff
108b18c0  0610a0e1  cpy r1, r6
108b18c4  0900a0e1  cpy r0, r9
108b18c8  f48afeeb  bl 0x108544a0
108b18cc  00b0a0e1  cpy r11, r0
108b18d0  0510a0e1  cpy r1, r5
108b18d4  0900a0e1  cpy r0, r9
108b18d8  f08afeeb  bl 0x108544a0
108b18dc  00a0a0e1  cpy r10, r0
108b18e0  0210a0e3  mov r1, #0x2
108b18e4  cd0affeb  bl 0x10874420
108b18e8  0070a0e1  cpy r7, r0
108b18ec  0410a0e3  mov r1, #0x4
108b18f0  0a00a0e1  cpy r0, r10
108b18f4  c90affeb  bl 0x10874420
108b18f8  0080a0e1  cpy r8, r0
108b18fc  0520a0e1  cpy r2, r5
108b1900  0e10a0e3  mov r1, #0xe
108b1904  0900a0e1  cpy r0, r9
108b1908  9be1feeb  bl 0x10869f7c
108b190c  0040a0e1  cpy r4, r0
108b1910  0450c0e5  strb r5, [r0,#0x4]
108b1914  0110a0e3  mov r1, #0x1
108b1918  0a00a0e1  cpy r0, r10
108b191c  bf0affeb  bl 0x10874420
108b1920  0500c4e5  strb r0, [r4,#0x5]
108b1924  0310a0e3  mov r1, #0x3
108b1928  0a00a0e1  cpy r0, r10
108b192c  bb0affeb  bl 0x10874420
108b1930  0700c4e5  strb r0, [r4,#0x7]
108b1934  0210a0e3  mov r1, #0x2
108b1938  0b00a0e1  cpy r0, r11
108b193c  b70affeb  bl 0x10874420
108b1940  0600c4e5  strb r0, [r4,#0x6]
108b1944  0410a0e3  mov r1, #0x4
108b1948  0b00a0e1  cpy r0, r11
108b194c  b30affeb  bl 0x10874420
108b1950  0800c4e5  strb r0, [r4,#0x8]
108b1954  0510a0e3  mov r1, #0x5
108b1958  0a00a0e1  cpy r0, r10
108b195c  af0affeb  bl 0x10874420
108b1960  0900c4e5  strb r0, [r4,#0x9]
108b1964  0610a0e3  mov r1, #0x6
108b1968  0a00a0e1  cpy r0, r10
108b196c  ab0affeb  bl 0x10874420
108b1970  0a00c4e5  strb r0, [r4,#0xa]
108b1974  0710a0e3  mov r1, #0x7
108b1978  0a00a0e1  cpy r0, r10
108b197c  a70affeb  bl 0x10874420
108b1980  0b00c4e5  strb r0, [r4,#0xb]
108b1984  0a00a0e1  cpy r0, r10
108b1988  5501d0e5  ldrb r0, [r0,#0x155]
108b198c  0c00c4e5  strb r0, [r4,#0xc]
108b1990  0410a0e1  cpy r1, r4
108b1994  0900a0e1  cpy r0, r9
108b1998  6ad8feeb  bl 0x10867b48
108b199c  0520a0e1  cpy r2, r5
108b19a0  0e10a0e3  mov r1, #0xe
108b19a4  0900a0e1  cpy r0, r9
108b19a8  73e1feeb  bl 0x10869f7c
108b19ac  0040a0e1  cpy r4, r0
108b19b0  0460c0e5  strb r6, [r0,#0x4]
108b19b4  0110a0e3  mov r1, #0x1
108b19b8  0b00a0e1  cpy r0, r11
108b19bc  970affeb  bl 0x10874420
108b19c0  0500c4e5  strb r0, [r4,#0x5]
108b19c4  0310a0e3  mov r1, #0x3
108b19c8  0b00a0e1  cpy r0, r11
108b19cc  930affeb  bl 0x10874420
108b19d0  0700c4e5  strb r0, [r4,#0x7]
108b19d4  0670c4e5  strb r7, [r4,#0x6]
108b19d8  0510a0e3  mov r1, #0x5
108b19dc  0b00a0e1  cpy r0, r11
108b19e0  0880c4e5  strb r8, [r4,#0x8]
108b19e4  8d0affeb  bl 0x10874420
108b19e8  0900c4e5  strb r0, [r4,#0x9]
108b19ec  0610a0e3  mov r1, #0x6
108b19f0  0b00a0e1  cpy r0, r11
108b19f4  890affeb  bl 0x10874420
108b19f8  0a00c4e5  strb r0, [r4,#0xa]
108b19fc  0710a0e3  mov r1, #0x7
108b1a00  0b00a0e1  cpy r0, r11
108b1a04  850affeb  bl 0x10874420
108b1a08  0b00c4e5  strb r0, [r4,#0xb]
108b1a0c  0b00a0e1  cpy r0, r11
108b1a10  5501d0e5  ldrb r0, [r0,#0x155]
108b1a14  0c00c4e5  strb r0, [r4,#0xc]
108b1a18  0410a0e1  cpy r1, r4
108b1a1c  0900a0e1  cpy r0, r9
108b1a20  48d8feeb  bl 0x10867b48
108b1a24  0520a0e1  cpy r2, r5
108b1a28  0310a0e3  mov r1, #0x3
108b1a2c  0900a0e1  cpy r0, r9
108b1a30  51e1feeb  bl 0x10869f7c
108b1a34  0040a0e1  cpy r4, r0
108b1a38  28209fe5  ldr r2, [0x108b1a68]
108b1a3c  0210a0e3  mov r1, #0x2
108b1a40  040080e2  add r0, r0, #0x4
108b1a44  226efeeb  bl 0x1084d2d4
108b1a48  0510a0e1  cpy r1, r5
108b1a4c  040084e2  add r0, r4, #0x4
108b1a50  116efeeb  bl 0x1084d29c
108b1a54  0410a0e1  cpy r1, r4
108b1a58  0900a0e1  cpy r0, r9
108b1a5c  f05fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108b1a60  38d8feea  b 0x10867b48
108b1a64  f09fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,pc}
