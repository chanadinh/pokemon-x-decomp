/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bab20(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = FUN_1086b91c(2);
  if (iVar1 == param_3) {
    uVar2 = FUN_108bb8ac(param_2,0);
    FUN_1084bef8(param_2,uVar2);
    return;
  }
  return;
}

