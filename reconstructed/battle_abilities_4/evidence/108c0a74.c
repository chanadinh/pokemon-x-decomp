/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c0a74(undefined4 param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(2);
  if ((iVar1 == param_3) && (iVar1 = FUN_1086b91c(0x45), iVar1 != 0)) {
    FUN_108658d4(0x36,0x2000);
    return;
  }
  return;
}

