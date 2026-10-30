/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined2 * FUN_0012b5d4(undefined2 *param_1)

{
  int iVar1;
  
  *param_1 = 0;
  *(undefined1 *)(param_1 + 1) = 0;
  *(undefined4 *)(param_1 + 2) = 0;
  *(undefined1 *)(param_1 + 4) = 0;
  iVar1 = FUN_00122920(0x68);
  if (iVar1 != 0) {
    FUN_0015e914(iVar1,0x68);
  }
  *(int *)(param_1 + 2) = iVar1;
  return param_1;
}

