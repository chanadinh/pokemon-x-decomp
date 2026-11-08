/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b6438(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    uVar3 = FUN_1086b91c(5);
    uVar4 = 0;
    if (uVar3 != 0) {
      do {
        uVar1 = FUN_1086b91c(uVar4 + 6 & 0xff);
        FUN_108b65f8(param_2,param_3,uVar1);
        uVar4 = uVar4 + 1;
      } while (uVar4 < uVar3);
    }
  }
  return;
}

