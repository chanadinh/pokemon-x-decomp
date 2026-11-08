/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108c21dc(int param_1,undefined4 param_2,int param_3)

{
  int iVar1;
  
  iVar1 = FUN_1086b91c(3);
  if (iVar1 == param_3) {
    *(code **)(param_1 + 0xc) = FUN_108c45d0;
    return;
  }
  return;
}

