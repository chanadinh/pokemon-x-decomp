/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 * FUN_0012b6a4(undefined4 *param_1)

{
  *param_1 = &DAT_00598a40;
  param_1[1] = 0xffffffff;
  FUN_00301a9c(param_1 + 2,0x194);
  return param_1;
}

