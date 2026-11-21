/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

void FUN_108704ac(uint *param_1,uint param_2,uint param_3)

{
  *param_1 = (param_2 & 7) << 4 | 3 | (param_3 & 7) << 7;
  return;
}

