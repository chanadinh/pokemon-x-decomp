/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c297c(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(3);
  if ((iVar1 == param_3) && (iVar1 = FUN_1086b91c(0x16), iVar1 == 2)) {
    iVar1 = FUN_1086b91c(0x19);
    FUN_108658d4(0x19,iVar1 + 1);
    return;
  }
  return;
}

