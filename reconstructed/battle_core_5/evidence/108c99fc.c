/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined1 FUN_108c99fc(int param_1,uint param_2)

{
  if (param_2 < 4) {
    return *(undefined1 *)(param_1 + param_2 * 0x31);
  }
  FUN_10800394(0,0);
  return 0;
}

