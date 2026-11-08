/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108a8b0c(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(4);
  if ((iVar1 == param_3) && (iVar1 = FUN_1086b91c(0x1b), iVar1 == 2)) {
    FUN_1084ce84(0x36,0x1800);
    return;
  }
  return;
}

