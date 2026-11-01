/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c0f10(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(4);
  if (((iVar1 == param_3) && (iVar1 = FUN_1086b91c(0x1b), iVar1 == 1)) &&
     (iVar1 = FUN_1084b8d4(param_2), iVar1 == 1)) {
    FUN_1084ce84(0x36,0x1800);
    return;
  }
  return;
}

