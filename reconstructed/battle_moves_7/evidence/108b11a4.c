/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b11a4(undefined4 param_1,undefined4 param_2,int param_3)

{
  undefined1 uVar1;
  int iVar2;
  
  iVar2 = FUN_1086b91c(3);
  if (iVar2 == param_3) {
    uVar1 = FUN_1086b91c(6);
    FUN_108b65f8(param_2,param_3,uVar1);
    return;
  }
  return;
}

