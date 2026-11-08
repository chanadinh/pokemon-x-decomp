/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b45bc(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  
  iVar5 = 0;
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    uVar3 = FUN_1086b91c(5);
    uVar4 = 0;
    if (uVar3 != 0) {
      do {
        uVar1 = FUN_1086b91c(uVar4 + 6 & 0xff);
        FUN_108544a0(param_2,uVar1);
        iVar2 = FUN_1085f844();
        if (iVar2 != 0) {
          iVar5 = iVar5 + 1;
        }
        uVar4 = uVar4 + 1;
      } while (uVar4 < uVar3);
      if (iVar5 != 0) {
        iVar2 = FUN_10869f7c(param_2,0xd,param_3);
        *(char *)(iVar2 + 0xc) = (char)param_3;
        *(undefined1 *)(iVar2 + 0x16) = 1;
        *(undefined1 *)(iVar2 + 4) = 1;
        *(undefined1 *)(iVar2 + 0x17) = 2;
        *(byte *)(iVar2 + 0x18) = *(byte *)(iVar2 + 0x18) | 4;
        FUN_10867b48(param_2);
        return;
      }
    }
  }
  return;
}

