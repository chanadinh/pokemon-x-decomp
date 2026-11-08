/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108bbb4c(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  
  iVar1 = FUN_1084bf14(param_2);
  if ((((iVar1 == 0) || (iVar1 = FUN_1084bf14(param_2), iVar1 == 3)) &&
      (iVar1 = FUN_1086b91c(2), iVar1 == param_3)) && (iVar1 = FUN_1086b91c(0x23), iVar1 == 0)) {
    FUN_108658d4(0x23,0x1b);
    return;
  }
  return;
}

