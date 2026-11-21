/* Unreviewed Ghidra pseudocode from live REA MCP; types require assembly review. */

undefined2 FUN_10855c68(int param_1,uint param_2)

{
  if (*(byte *)(param_1 + 0x152) <= param_2) {
    FUN_10800394(0,0);
  }
  return *(undefined2 *)(param_1 + param_2 * 0xe + 0x11c);
}

