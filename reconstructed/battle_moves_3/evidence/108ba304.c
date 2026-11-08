/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108ba304(int param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(3);
  if (iVar1 == param_3) {
    *(undefined **)(param_1 + 0xc) = &DAT_108baba4;
    return;
  }
  return;
}

