/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c43a0(undefined4 param_1,undefined4 param_2,int param_3)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  
  uVar1 = FUN_1086b91c(5);
  uVar3 = 0;
  if (uVar1 != 0) {
    do {
      iVar2 = FUN_1086b91c(uVar3 + 6 & 0xff);
      if (iVar2 == param_3) {
        iVar2 = FUN_1086b91c(5);
        if (iVar2 != 1) {
          return;
        }
        FUN_108658d4(0x48,1);
        return;
      }
      uVar3 = uVar3 + 1;
    } while (uVar3 < uVar1);
  }
  return;
}

