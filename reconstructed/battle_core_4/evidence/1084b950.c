/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

int FUN_1084b950(int param_1,undefined4 param_2)

{
  uint uVar1;
  int iVar2;
  
  uVar1 = FUN_1087425c(param_2);
  iVar2 = *(int *)(param_1 + 8);
  if (3 < uVar1) {
    FUN_10800394(0,0);
  }
  return iVar2 + uVar1 * 0x1c + 4;
}

