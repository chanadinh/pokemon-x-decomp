/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

bool FUN_10856a88(int param_1,uint param_2)

{
  if (0x25 < param_2) {
    FUN_10800394(0,0);
  }
  return (*(uint *)(param_1 + param_2 * 4 + 0x1c) & 7) != 0;
}

