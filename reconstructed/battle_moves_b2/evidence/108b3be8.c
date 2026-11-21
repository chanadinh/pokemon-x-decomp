/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b3be8(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  undefined2 unaff_r9;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    uVar3 = FUN_1086b91c(5);
    if (uVar3 != 0) {
      unaff_r9 = 0x494a;
    }
    uVar4 = 0;
    if (uVar3 != 0) {
      do {
        iVar2 = FUN_10869f7c(param_2,0x13,param_3);
        *(undefined2 *)(iVar2 + 4) = unaff_r9;
        uVar1 = FUN_1086b91c(uVar4 + 6 & 0xff);
        *(undefined1 *)(iVar2 + 6) = uVar1;
        *(undefined1 *)(iVar2 + 8) = 1;
        FUN_10867b48(param_2,iVar2);
        uVar4 = uVar4 + 1;
      } while (uVar4 < uVar3);
    }
  }
  return;
}

