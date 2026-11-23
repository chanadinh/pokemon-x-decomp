/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

bool FUN_108c8400(int param_1,uint param_2)

{
  if (param_2 < 4) {
    return *(int *)(*(int *)(param_1 + 4) + param_2 * 0x38 + 0x83c) != 0;
  }
  FUN_10800394(0,0);
  return false;
}

