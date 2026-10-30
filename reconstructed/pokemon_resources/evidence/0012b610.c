/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 * FUN_0012b610(undefined4 *param_1)

{
  int iVar1;
  
  *(undefined2 *)(param_1 + 1) = 0;
  param_1[2] = 0;
  *param_1 = &DAT_005989d0;
  iVar1 = FUN_00122920(0x30);
  if (iVar1 != 0) {
    FUN_0015e914(iVar1,0x30);
  }
  param_1[2] = iVar1;
  return param_1;
}

