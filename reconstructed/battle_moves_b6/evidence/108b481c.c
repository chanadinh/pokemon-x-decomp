/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b481c(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  char cVar2;
  int iVar3;
  undefined4 uVar4;
  uint uVar5;
  
  iVar3 = FUN_1086b91c(3);
  if (iVar3 == param_3) {
    uVar1 = FUN_1086b91c(6);
    uVar4 = FUN_108544a0(param_2,uVar1);
    uVar5 = 0;
    cVar2 = '\0';
    do {
      iVar3 = FUN_108722e8(uVar4,(&DAT_108d2c46)[uVar5],2);
      if (iVar3 != 0) {
        cVar2 = cVar2 + '\x01';
      }
      uVar5 = uVar5 + 1 & 0xff;
    } while (uVar5 < 7);
    if (cVar2 != '\0') {
      cVar2 = FUN_1084c7bc(cVar2);
      uVar5 = 0;
      do {
        iVar3 = FUN_108722e8(uVar4,(&DAT_108d2c46)[uVar5],2);
        if (iVar3 != 0) {
          if (cVar2 == '\0') {
            iVar3 = FUN_10869f7c(param_2,0xd,param_3);
            *(undefined1 *)(iVar3 + 0xc) = uVar1;
            *(undefined1 *)(iVar3 + 0x16) = 1;
            uVar1 = (&DAT_108d2c46)[uVar5];
            *(undefined1 *)(iVar3 + 0x17) = 2;
            *(undefined1 *)(iVar3 + 4) = uVar1;
            *(byte *)(iVar3 + 0x18) = *(byte *)(iVar3 + 0x18) & 0xfd | 0x14;
            FUN_10867b48(param_2);
            return;
          }
          cVar2 = cVar2 + -1;
        }
        uVar5 = uVar5 + 1 & 0xff;
      } while (uVar5 < 7);
    }
  }
  return;
}

