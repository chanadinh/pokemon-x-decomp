/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108b67b0(undefined4 param_1,undefined4 param_2,int param_3,int *param_4)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(3);
  if ((iVar1 == param_3) && (*param_4 != 0)) {
    FUN_108658d4(0x31,0x96);
    return;
  }
  return;
}

