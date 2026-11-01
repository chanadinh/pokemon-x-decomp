/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_1084d2d4(undefined2 *param_1,ushort param_2,undefined2 param_3)

{
  *param_1 = param_3;
  param_1[1] = param_2 & 0x7f | param_1[1] & 0xc000;
  return;
}

