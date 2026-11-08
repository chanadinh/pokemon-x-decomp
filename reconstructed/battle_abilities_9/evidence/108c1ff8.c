/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c1ff8(undefined4 param_1,undefined4 param_2,int param_3,int param_4)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(3);
  if ((iVar1 == param_3) && (iVar1 = FUN_1086b91c(4), iVar1 == param_3)) {
    *(undefined4 *)(param_4 + 0x10) = 1;
  }
  return;
}

