/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined4 FUN_108c84e0(int param_1,uint param_2)

{
  if (0x25 < param_2) {
    FUN_10800394(0,0);
  }
  return *(undefined4 *)(param_1 + param_2 * 4 + 0x1c);
}

